/* DICU Tudor - 313CD */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

/* Structura pentru echipajul de interventie */
typedef struct unit {
    int id, disp;
    char tip;
} unit;

/* Structura pentru incident (nod de lista dublu inlantuita circulara) */
typedef struct incident {
    int id;
    char priority[7], *description, status[11];
    struct incident *next, *prev;
} incident;

/* Structura pentru interventie (face legatura intre incident si unitate) */
typedef struct intervention {
    incident *incident;
    unit *unit;
    struct intervention *next, *prev;
} intervention;

/* Hub-ul central al sistemului. Contine listele, vectorul si cozile/stiva */
typedef struct system_112 {
    incident *incidents;
    intervention *interventions;
    unit *units;
    int num_units;
    queue q_high, q_med, q_low, q_avail;
    stack history;
} system_112;

/* Parcurge o coada pentru a-i afla numarul de elemente */
int get_queue_size(queue *q)
{
    int count = 0;
    node *curr = q->head;

    while (curr) {
        count++;
        curr = curr->next;
    }

    return count;
}

/* Adauga un element la INCEPUTUL cozii (esential pentru comanda UNDO) */
void enqueue_front(queue *q, void *data)
{
    node *n = (node *)malloc(sizeof(node));

    if (!n)
        return;

    n->data = data;
    n->next = q->head;
    q->head = n;

    if (!q->tail)
        q->tail = n;
}

/* Creeaza nodul "fictiv" (santinela) pentru lista circulara de incidente */
incident *init_incident_sentinel(void)
{
    incident *s = (incident *)malloc(sizeof(incident));

    if (!s)
        return NULL;

    s->id = 0;
    strcpy(s->priority, "low");
    s->description = strdup("test incident");
    strcpy(s->status, "solved");
    /* Fiind singurul element la inceput, arata spre el insusi */
    s->next = s;
    s->prev = s;

    return s;
}

/* Creeaza santinela pentru lista circulara de interventii */
intervention *init_intervention_sentinel(void)
{
    intervention *s = (intervention *)malloc(sizeof(intervention));

    if (!s)
        return NULL;

    s->incident = NULL;
    s->unit = NULL;
    s->next = s;
    s->prev = s;

    return s;
}

/* Citeste si aloca dinamic descrierea unui incident, ignorand ghilimelele */
char *get_description(FILE *in)
{
    char buf[256];
    int len;

    fgetc(in);
    fgetc(in);
    fgets(buf, 256, in);

    len = strlen(buf);
    if (buf[len - 1] == '\n') {
        buf[len - 1] = '\0';
        len--;
    }
    if (buf[len - 1] == '\"')
        buf[len - 1] = '\0';

    return strdup(buf);
}

/* Citeste incidentul si il pune atat in lista mare cat si in coada specifica */
void do_add_incident(system_112 *sys, FILE *in)
{
    incident *new_inc = (incident *)malloc(sizeof(incident));

    fscanf(in, "%d %s", &new_inc->id, new_inc->priority);
    new_inc->description = get_description(in);
    strcpy(new_inc->status, "queued");

    /* Inserare la finalul listei circulare (inainte de santinela) */
    new_inc->next = sys->incidents;
    new_inc->prev = sys->incidents->prev;
    sys->incidents->prev->next = new_inc;
    sys->incidents->prev = new_inc;

    /* Repartizare in coada corecta de prioritati */
    if (strcmp(new_inc->priority, "high") == 0)
        enqueue(&sys->q_high, new_inc);
    else if (strcmp(new_inc->priority, "medium") == 0)
        enqueue(&sys->q_med, new_inc);
    else
        enqueue(&sys->q_low, new_inc);
}

/* Inima sistemului: asigneaza cel mai prioritar incident unui echipaj */
void do_dispatch(system_112 *sys, FILE *out)
{
    incident *inc = NULL;
    unit *u = NULL;
    intervention *interv;

    /* Daca nu avem echipaje sau nu avem niciun incident in vreo coada */
    if (!sys->q_avail.head || (!sys->q_high.head && !sys->q_med.head &&
        !sys->q_low.head)) {
        fprintf(out, "INVALID OPERATION! ERROR 404\n");
        return;
    }

    /* Alegem incidentul tinand cont de prioritate (Cascada If-Else) */
    if (sys->q_high.head)
        inc = (incident *)dequeue(&sys->q_high);
    else if (sys->q_med.head)
        inc = (incident *)dequeue(&sys->q_med);
    else
        inc = (incident *)dequeue(&sys->q_low);

    u = (unit *)dequeue(&sys->q_avail);
    strcpy(inc->status, "intervened");
    u->disp = 0;

    /* Cream legatura (interventia) intre ele */
    interv = (intervention *)malloc(sizeof(intervention));
    interv->incident = inc;
    interv->unit = u;

    /* Adaugam interventia in lista circulara */
    interv->next = sys->interventions;
    interv->prev = sys->interventions->prev;
    sys->interventions->prev->next = interv;
    sys->interventions->prev = interv;

    /* O salvam in istoric pentru posibile UNDO-uri ulterioare */
    push(&sys->history, interv);
}

/* Anuleaza ultima interventie in desfasurare */
void do_undo(system_112 *sys, FILE *out)
{
    intervention *interv = NULL;

    /* Cautam in stiva ultima interventie care NU a fost inca "solved" */
    while (sys->history.top) {
        interv = (intervention *)pop(&sys->history);
        if (strcmp(interv->incident->status, "intervened") == 0)
            break;
        interv = NULL;
    }

    if (!interv) {
        fprintf(out, "INVALID OPERATION! ERROR 404\n");
        return;
    }

    /* Restauram statusurile initiale */
    strcpy(interv->incident->status, "queued");
    interv->unit->disp = 1;

    /* Incidentul trebuie sa se intoarca la INCEPUTUL cozii sale, 
       ca sa fie primul servit data viitoare */
    if (strcmp(interv->incident->priority, "high") == 0)
        enqueue_front(&sys->q_high, interv->incident);
    else if (strcmp(interv->incident->priority, "medium") == 0)
        enqueue_front(&sys->q_med, interv->incident);
    else
        enqueue_front(&sys->q_low, interv->incident);

    /* Echipajul revine disponibil */
    enqueue(&sys->q_avail, interv->unit);

    /* Scoatem interventia din lista dubla circulara si o stergem complet */
    interv->prev->next = interv->next;
    interv->next->prev = interv->prev;
    free(interv);
}

/* Marcheaza o interventie ca fiind finalizata */
void do_solved(system_112 *sys, int id, FILE *out)
{
    incident *inc = sys->incidents->next;
    intervention *interv = sys->interventions->next;
    int found = 0;

    /* Cautam incidentul in lista */
    while (inc != sys->incidents) {
        if (inc->id == id) {
            found = 1;
            break;
        }
        inc = inc->next;
    }

    if (!found || strcmp(inc->status, "intervened") != 0) {
        fprintf(out, "INVALID OPERATION! ERROR 404\n");
        return;
    }

    strcpy(inc->status, "solved");

    /* Cautam echipajul asociat pentru a-l elibera */
    while (interv != sys->interventions) {
        if (interv->incident->id == id) {
            interv->unit->disp = 1;
            enqueue(&sys->q_avail, interv->unit);
            break;
        }
        interv = interv->next;
    }
}

/* Afisari (functii pretty-print) */
void do_show_unit(system_112 *sys, int id, FILE *out)
{
    int found = 0;
    unit *u = NULL;

    for (int i = 0; i < sys->num_units; i++) {
        if (sys->units[i].id == id) {
            found = 1;
            u = &sys->units[i];
            break;
        }
    }

    if (!found) {
        fprintf(out, "INVALID OPERATION! ERROR 404\n");
        return;
    }

    fprintf(out, "Unit %d is type %c and is %s\n", u->id, u->tip,
        u->disp ? "available" : "unavailable");
}

void do_show_incident(system_112 *sys, int id, FILE *out)
{
    incident *inc = sys->incidents->next;
    int found = 0;

    while (inc != sys->incidents) {
        if (inc->id == id) {
            found = 1;
            break;
        }
        inc = inc->next;
    }

    if (!found) {
        fprintf(out, "INVALID OPERATION! ERROR 404\n");
        return;
    }

    fprintf(out, "Incident %d has %s priority, the following description: "
        "\"%s\" and is %s\n", inc->id, inc->priority, inc->description,
        inc->status);
}

void do_show_interventions(system_112 *sys, FILE *out)
{
    intervention *interv = sys->interventions->next;

    if (interv == sys->interventions) {
        fprintf(out, "No intervention has been initiated\n");
        return;
    }

    while (interv != sys->interventions) {
        fprintf(out, "Incident %d was assigned to unit %d, "
            "and has the following status: \"%s\"\n",
            interv->incident->id, interv->unit->id,
            interv->incident->status);
        interv = interv->next;
    }
}

void cleanup_system(system_112 *sys)
{
    incident *curr_inc = sys->incidents->next, *temp_inc;
    intervention *curr_int = sys->interventions->next, *temp_int;

    while (dequeue(&sys->q_high));
    while (dequeue(&sys->q_med));
    while (dequeue(&sys->q_low));
    while (dequeue(&sys->q_avail));
    while (pop(&sys->history));

    while (curr_inc != sys->incidents) {
        temp_inc = curr_inc;
        curr_inc = curr_inc->next;
        free(temp_inc->description);
        free(temp_inc);
    }
    free(sys->incidents->description);
    free(sys->incidents);

    while (curr_int != sys->interventions) {
        temp_int = curr_int;
        curr_int = curr_int->next;
        free(temp_int);
    }
    free(sys->interventions);
    free(sys->units);
}

int main(void)
{
    FILE *in = fopen("tema1.in", "r");
    FILE *out = fopen("tema1.out", "w");
    system_112 sys;
    int num_ops, id;
    char cmd[32];

    if (!in || !out)
        return 1;

    sys.q_high.head = sys.q_high.tail = NULL;
    sys.q_med.head = sys.q_med.tail = NULL;
    sys.q_low.head = sys.q_low.tail = NULL;
    sys.q_avail.head = sys.q_avail.tail = NULL;
    sys.history.top = NULL;

    sys.incidents = init_incident_sentinel();
    sys.interventions = init_intervention_sentinel();

    fscanf(in, "%d", &sys.num_units);
    sys.units = (unit *)malloc(sys.num_units * sizeof(unit));

    for (int i = 0; i < sys.num_units; i++) {
        fscanf(in, "%d %c", &sys.units[i].id, &sys.units[i].tip);
        sys.units[i].disp = 1;
        enqueue(&sys.q_avail, &sys.units[i]);
    }

    fscanf(in, "%d", &num_ops);

    for (int i = 0; i < num_ops; i++) {
        fscanf(in, "%s", cmd);

        if (strcmp(cmd, "ADD_INCIDENT") == 0)
            do_add_incident(&sys, in);
        else if (strcmp(cmd, "CHECK_UNITS_AVAILABILITY") == 0)
            fprintf(out, "Number of available units: %d\n",
                get_queue_size(&sys.q_avail));
        else if (strcmp(cmd, "DISPATCH") == 0)
            do_dispatch(&sys, out);
        else if (strcmp(cmd, "UNDO_LAST_DISPATCH") == 0)
            do_undo(&sys, out);
        else if (strcmp(cmd, "SOLVED_INCIDENT") == 0) {
            fscanf(in, "%d", &id);
            do_solved(&sys, id, out);
        }
        else if (strcmp(cmd, "SHOW_UNIT") == 0) {
            fscanf(in, "%d", &id);
            do_show_unit(&sys, id, out);
        }
        else if (strcmp(cmd, "SHOW_INCIDENT") == 0) {
            fscanf(in, "%d", &id);
            do_show_incident(&sys, id, out);
        }
        else if (strcmp(cmd, "SHOW_INTERVENTIONS") == 0)
            do_show_interventions(&sys, out);
    }

    cleanup_system(&sys);
    fclose(in);
    fclose(out);

    return 0;
}