#define _GNU_SOURCE
#include <ctype.h>	/* isdigit */
#include <stdlib.h>	/* atoi, qsort */
#include <stdio.h>	/* asprintf */
#include <string.h>
#include <math.h>
#include "abc.h"
#include "parser.h"

signed char _pitch_diff_CMaj_0x3c['h'] = {
    /* CMaj */
    ['A'] = 9,
    ['B'] = 11,
    ['C'] = 0,
    ['D'] = 2,
    ['E'] = 4,
    ['F'] = 5,
    ['G'] = 7,

    ['a'] = 9 + 12,
    ['b'] = 11 + 12,
    ['c'] = 0 + 12,
    ['d'] = 2 + 12,
    ['e'] = 4 + 12,
    ['f'] = 5 + 12,
    ['g'] = 7 + 12
};

signed char _pitch_diff_GMaj_0x3c['h'] = {
    /* GMaj */
    ['A'] = 9,
    ['B'] = 11,
    ['C'] = 0,
    ['D'] = 2,
    ['E'] = 4,
    ['F'] = 6,
    ['G'] = 7,

    ['a'] = 9 + 12,
    ['b'] = 11 + 12,
    ['c'] = 0 + 12,
    ['d'] = 2 + 12,
    ['e'] = 4 + 12,
    ['f'] = 6 + 12,
    ['g'] = 7 + 12
};

signed char _pitch_diff_DMaj_0x3c['h'] = {
    /* DMaj */
    ['A'] = 9,
    ['B'] = 11,
    ['C'] = 1,
    ['D'] = 2,
    ['E'] = 4,
    ['F'] = 6,
    ['G'] = 7,

    ['a'] = 9 + 12,
    ['b'] = 11 + 12,
    ['c'] = 1 + 12,
    ['d'] = 2 + 12,
    ['e'] = 4 + 12,
    ['f'] = 6 + 12,
    ['g'] = 7 + 12
};

signed char _pitch_diff_AMaj_0x3c['h'] = {
    /* AMaj */
    ['A'] = 9,
    ['B'] = 11,
    ['C'] = 1,
    ['D'] = 2,
    ['E'] = 4,
    ['F'] = 6,
    ['G'] = 8,

    ['a'] = 9 + 12,
    ['b'] = 11 + 12,
    ['c'] = 1 + 12,
    ['d'] = 2 + 12,
    ['e'] = 4 + 12,
    ['f'] = 6 + 12,
    ['g'] = 8 + 12
};

signed char _pitch_diff_EMaj_0x3c['h'] = {
    /* EMaj */
    ['A'] = 9,
    ['B'] = 11,
    ['C'] = 1,
    ['D'] = 3,
    ['E'] = 4,
    ['F'] = 6,
    ['G'] = 8,

    ['a'] = 9 + 12,
    ['b'] = 11 + 12,
    ['c'] = 1 + 12,
    ['d'] = 3 + 12,
    ['e'] = 4 + 12,
    ['f'] = 6 + 12,
    ['g'] = 8 + 12
};

signed char _pitch_diff_BMaj_0x3c['h'] = {
    /* BMaj */
    ['A'] = 10,
    ['B'] = 11,
    ['C'] = 1,
    ['D'] = 3,
    ['E'] = 4,
    ['F'] = 6,
    ['G'] = 8,

    ['a'] = 10 + 12,
    ['b'] = 11 + 12,
    ['c'] = 1 + 12,
    ['d'] = 3 + 12,
    ['e'] = 4 + 12,
    ['f'] = 6 + 12,
    ['g'] = 8 + 12
};

signed char _pitch_diff_FSharpMaj_0x3c['h'] = {
    /* FSharpMaj */
    ['A'] = 10,
    ['B'] = 11,
    ['C'] = 1,
    ['D'] = 3,
    ['E'] = 5,
    ['F'] = 6,
    ['G'] = 8,

    ['a'] = 10 + 12,
    ['b'] = 11 + 12,
    ['c'] = 1 + 12,
    ['d'] = 3 + 12,
    ['e'] = 5 + 12,
    ['f'] = 6 + 12,
    ['g'] = 8 + 12
};

signed char _pitch_diff_GFlatMaj_0x3c['h'] = {
    /* GFlatMaj */
    ['A'] = 8,
    ['B'] = 10,
    ['C'] = -1,
    ['D'] = 1,
    ['E'] = 3,
    ['F'] = 5,
    ['G'] = 6,

    ['a'] = 8 + 12,
    ['b'] = 10 + 12,
    ['c'] = -1 + 12,
    ['d'] = 1 + 12,
    ['e'] = 3 + 12,
    ['f'] = 5 + 12,
    ['g'] = 6 + 12
};

signed char _pitch_diff_CSharpMaj_0x3c['h'] = {
    /* CSharpMaj */
    ['A'] = 10,
    ['B'] = 12,
    ['C'] = 1,
    ['D'] = 3,
    ['E'] = 5,
    ['F'] = 6,
    ['G'] = 8,

    ['a'] = 10 + 12,
    ['b'] = 12 + 12,
    ['c'] = 1 + 12,
    ['d'] = 3 + 12,
    ['e'] = 5 + 12,
    ['f'] = 6 + 12,
    ['g'] = 8 + 12
};

signed char _pitch_diff_DFlatMaj_0x3c['h'] = {
    /* DFlatMaj */
    ['A'] = 8,
    ['B'] = 10,
    ['C'] = 0,
    ['D'] = 1,
    ['E'] = 3,
    ['F'] = 5,
    ['G'] = 6,

    ['a'] = 8 + 12,
    ['b'] = 10 + 12,
    ['c'] = 0 + 12,
    ['d'] = 1 + 12,
    ['e'] = 3 + 12,
    ['f'] = 5 + 12,
    ['g'] = 6 + 12
};

signed char _pitch_diff_AFlatMaj_0x3c['h'] = {
    /* AFlatMaj */
    ['A'] = 8,
    ['B'] = 10,
    ['C'] = 0,
    ['D'] = 1,
    ['E'] = 3,
    ['F'] = 5,
    ['G'] = 7,

    ['a'] = 8 + 12,
    ['b'] = 10 + 12,
    ['c'] = 0 + 12,
    ['d'] = 1 + 12,
    ['e'] = 3 + 12,
    ['f'] = 5 + 12,
    ['g'] = 7 + 12
};

signed char _pitch_diff_EFlatMaj_0x3c['h'] = {
    /* EFlatMaj */
    ['A'] = 8,
    ['B'] = 10,
    ['C'] = 0,
    ['D'] = 2,
    ['E'] = 3,
    ['F'] = 5,
    ['G'] = 7,

    ['a'] = 8 + 12,
    ['b'] = 10 + 12,
    ['c'] = 0 + 12,
    ['d'] = 2 + 12,
    ['e'] = 3 + 12,
    ['f'] = 5 + 12,
    ['g'] = 7 + 12
};

signed char _pitch_diff_BFlatMaj_0x3c['h'] = {
    /* BFlatMaj */
    ['A'] = 9,
    ['B'] = 10,
    ['C'] = 0,
    ['D'] = 2,
    ['E'] = 3,
    ['F'] = 5,
    ['G'] = 7,

    ['a'] = 9 + 12,
    ['b'] = 10 + 12,
    ['c'] = 0 + 12,
    ['d'] = 2 + 12,
    ['e'] = 3 + 12,
    ['f'] = 5 + 12,
    ['g'] = 7 + 12
};

signed char _pitch_diff_FMaj_0x3c['h'] = {
    /* FMaj */
    ['A'] = 9,
    ['B'] = 10,
    ['C'] = 0,
    ['D'] = 2,
    ['E'] = 4,
    ['F'] = 5,
    ['G'] = 7,

    ['a'] = 9 + 12,
    ['b'] = 10 + 12,
    ['c'] = 0 + 12,
    ['d'] = 2 + 12,
    ['e'] = 4 + 12,
    ['f'] = 5 + 12,
    ['g'] = 7 + 12
};

/* semitones between note over C or c. */
unsigned char pitch_diff_0x3c(const char* ks, int note) {
    if (!ks || !strcasecmp(ks, "") || !strcasecmp(ks, "C") || !strcasecmp(ks, "Cmaj") || !strcasecmp(ks, "Amin") /* || !strcasecmp(ks, "Ddor") ... */ )
        return _pitch_diff_CMaj_0x3c[note];

    if (!strcasecmp(ks, "G") || !strcasecmp(ks, "Gmaj") || !strcasecmp(ks, "Emin"))
        return _pitch_diff_GMaj_0x3c[note];

    if (!strcasecmp(ks, "D") || !strcasecmp(ks, "Dmaj") || !strcasecmp(ks, "Bmin"))
        return _pitch_diff_DMaj_0x3c[note];

    if (!strcasecmp(ks, "A") || !strcasecmp(ks, "Amaj") || !strcasecmp(ks, "F#min"))
        return _pitch_diff_AMaj_0x3c[note];

    if (!strcasecmp(ks, "E") || !strcasecmp(ks, "Emaj") || !strcasecmp(ks, "C#min"))
        return _pitch_diff_EMaj_0x3c[note];

    if (!strcasecmp(ks, "B") || !strcasecmp(ks, "Bmaj") || !strcasecmp(ks, "G#min"))
        return _pitch_diff_BMaj_0x3c[note];

    if (!strcasecmp(ks, "F#") || !strcasecmp(ks, "F#maj") || !strcasecmp(ks, "D#min"))
        return _pitch_diff_FSharpMaj_0x3c[note];

    if (!strcasecmp(ks, "Gb") || !strcasecmp(ks, "Gbmaj") || !strcasecmp(ks, "Ebmin"))
        return _pitch_diff_GFlatMaj_0x3c[note];

    if (!strcasecmp(ks, "C#") || !strcasecmp(ks, "C#maj") || !strcasecmp(ks, "A#min"))
        return _pitch_diff_CSharpMaj_0x3c[note];

    if (!strcasecmp(ks, "Db") || !strcasecmp(ks, "Dbmaj") || !strcasecmp(ks, "Bbmin"))
        return _pitch_diff_DFlatMaj_0x3c[note];

    if (!strcasecmp(ks, "Ab") || !strcasecmp(ks, "Abmaj") || !strcasecmp(ks, "Fmin"))
        return _pitch_diff_AFlatMaj_0x3c[note];

    if (!strcasecmp(ks, "Eb") || !strcasecmp(ks, "Ebmaj") || !strcasecmp(ks, "Cmin"))
        return _pitch_diff_EFlatMaj_0x3c[note];

    if (!strcasecmp(ks, "Bb") || !strcasecmp(ks, "Bbmaj") || !strcasecmp(ks, "Gmin"))
        return _pitch_diff_BFlatMaj_0x3c[note];

    if (!strcasecmp(ks, "F") || !strcasecmp(ks, "Fmaj") || !strcasecmp(ks, "Dmin"))
        return _pitch_diff_FMaj_0x3c[note];

    /* default */
    return _pitch_diff_CMaj_0x3c[note];
}

/* converts a note text to a MIDI key */
unsigned char note2key(const char* keysig, const char* note, int* measure_accid) {
    int octava = 0;
    for (const char* c = note; *c; c++) {
        if (*c == ',')
            octava--;
        else if (*c == '\'')
            octava++;
    }

#define ACCID_NATURAL 0xff

    int accid = 0;
    for (const char* c = note; *c && !isalpha(*c); c++) {
        if (*c == '_')
            accid--;
        else if (*c == '^')
            accid++;
        else if (*c == '=')
            accid = ACCID_NATURAL;
    }

    unsigned char pitch = 0;
    const char *c;

    for (c = note; *c && !isalpha(*c); c++) {;}

    if ((*c >= 'A' && *c <= 'G') || (*c >= 'a' && *c <= 'g')) {
        if (accid) {
            measure_accid[(int)tolower(*c)] = accid;
            measure_accid[(int)toupper(*c)] = accid;
        }

        if (accid == ACCID_NATURAL)
            pitch = pitch_diff_0x3c("Cmaj", *c) + 0 + (octava * 12);
        else if (!accid) {
            if (measure_accid[(int)*c] == ACCID_NATURAL)
                pitch = pitch_diff_0x3c("Cmaj", *c) + 0 + (octava * 12);
            else
                pitch = pitch_diff_0x3c(keysig, *c) + measure_accid[(int)*c] + (octava * 12);
        } else
            pitch = pitch_diff_0x3c("Cmaj", *c) + accid + (octava * 12);

        pitch += 0x3c; /* MIDI key for C */
    }

    return pitch;
}

static
int smf_key_signature(const char* text, int* mode) {
    *mode = 0;
    if (strstr(text, "min"))
        *mode = 1;

    if (!text || !strcmp(text, "C") || !strcmp(text, "Cmaj") || !strcmp(text, "Amin"))
        return 0;

    if (!strcmp(text, "G") || !strcmp(text, "Gmaj") || !strcmp(text, "Emin"))
        return 1;

    if (!strcmp(text, "D") || !strcmp(text, "Dmaj") || !strcmp(text, "Bmin"))
        return 2;

    if (!strcmp(text, "A") || !strcmp(text, "Amaj") || !strcmp(text, "F#min"))
        return 3;

    if (!strcmp(text, "E") || !strcmp(text, "Emaj") || !strcmp(text, "C#min"))
        return 4;

    if (!strcmp(text, "B") || !strcmp(text, "Bmaj") || !strcmp(text, "G#min"))
        return 5;

    if (!strcmp(text, "F#") || !strcmp(text, "F#maj") || !strcmp(text, "D#min"))
        return 6;

    if (!strcmp(text, "C#") || !strcmp(text, "C#maj") || !strcmp(text, "A#min"))
        return 7;


    if (!strcmp(text, "F") || !strcmp(text, "Fmaj") || !strcmp(text, "Dmin"))
        return -1;

    if (!strcmp(text, "Bb") || !strcmp(text, "Bbmaj") || !strcmp(text, "Gmin"))
        return -2;

    if (!strcmp(text, "Eb") || !strcmp(text, "Ebmaj") || !strcmp(text, "Cmin"))
        return -3;

    if (!strcmp(text, "Ab") || !strcmp(text, "Abmaj") || !strcmp(text, "Fmin"))
        return -4;

    if (!strcmp(text, "Db") || !strcmp(text, "Dbmaj") || !strcmp(text, "Bbmin"))
        return -5;

    if (!strcmp(text, "Gb") || !strcmp(text, "Gbmaj") || !strcmp(text, "Ebmin"))
        return -6;

    if (!strcmp(text, "Cb") || !strcmp(text, "Cbmaj") || !strcmp(text, "Abmin"))
        return -7;

    return 0;
}

void abc_tune_append(struct abc* yy, const char* yytext)
{
    struct abc_tune* new = calloc(1, sizeof (struct abc_tune));
    new->x = atoi(yytext);

    yy->tunes = realloc(yy->tunes, sizeof (struct abc_tune*) * (yy->count + 1));
    yy->tunes[yy->count] = new;
    yy->count++;
}

void abc_header_append(struct abc* yy, const char* yytext, const char which)
{
    struct abc_tune* tune = yy->tunes[yy->count-1];
    struct abc_header* new = calloc(1, sizeof (struct abc_header));

    if (!tune->headers) {
        tune->headers = new;
    } else {
        struct abc_header* h;
        for (h = tune->headers; h->next; h = h->next) {;}

        h->next = new;
    }

    new->h = which;
    new->text = strdup(yytext);

    if (which == 'K')
        yy->ks = new->text;
    else if (which == 'L')
        yy->ul = new->text;
    else if (which == 'M')
        yy->mm = new->text;
}

void abc_voice_append(struct abc* yy, const char* yytext)
{
    struct abc_voice* new = calloc(1, sizeof (struct abc_voice));
    new->v = strdup(yytext);

    struct abc_tune* tune = yy->tunes[yy->count-1];
    tune->voices = realloc(tune->voices, sizeof (struct abc_voice*) * (tune->count + 1));
    tune->voices[tune->count] = new;
    tune->count++;
}

struct abc_symbol* abc_last_symbol(struct abc* yy)
{
    struct abc_tune* tune = yy->tunes[yy->count-1];
    if (tune->count == 0) /* voice 1 can be implicit */
        abc_voice_append(yy, "1");

    struct abc_voice* voice = tune->voices[tune->count-1];

    return voice->last;
}

struct abc_symbol* abc_new_symbol(struct abc* yy)
{
    struct abc_symbol* new = calloc(1, sizeof (struct abc_symbol));
    new->ev.start_den = 1;
    new->dur_den = 1;

    struct abc_tune* tune = yy->tunes[yy->count-1];
    if (tune->count == 0) /* voice 1 can be implicit */
        abc_voice_append(yy, "1");

    struct abc_voice* voice = tune->voices[tune->count-1];

    if (voice->last == NULL) {
        voice->last = new;
        voice->first = voice->last;
    } else {
        struct abc_symbol* s = voice->last;
        voice->last = new;
        s->next = new;
        new->prev = s;
        new->index = s->index + 1;
    }

    return new;
}

void abc_instruction_append(struct abc* yy, const char* yytext)
{
    struct abc_symbol* new = abc_new_symbol(yy);
    new->kind = ABC_INST;
    new->text = strdup(yytext);
}

void abc_lyrics_append(struct abc* yy, const char* yytext)
{
    char** syllables = NULL;
    int count = 0;
    int i = 0;
    int in = 0;
    const char* b;
    const char* e;

    while (yytext[i]) {
        if (isspace(yytext[i])) {
            in = 0;
        } else {
            if (!in) {
                in = 1;
                count++;
                b = &yytext[i];
                for (e = b; *e && !isspace(*e); e++);

                char* text = malloc(e-b +1);
                strncpy(text, b, e-b);
                text[e-b] = 0;

                syllables = realloc(syllables, count * sizeof (char*));
                syllables[count-1] = text;

                i += (e - b - 1);
            }
        }

        i++;
    }

    struct abc_symbol* s =  abc_last_symbol(yy);
    /* rewind to last note */
    while (s->kind != ABC_NOTE && s->prev)
        s = s->prev;

    int nc = 1;
    /* rewind to first symbol of this line */
    while (s->kind != ABC_EOL && s->prev) {
        s = s->prev;
        if (s->kind == ABC_NOTE)
            nc++;
    }

    int min = nc < count ? nc : count;
    for (i = 0; i < min; i++) {
        while (s->kind != ABC_NOTE && s->next)
            s = s->next;

        s->lyr = syllables[i];
        if (s->next)
            s = s->next;
    }

    free(syllables);
}

void abc_eol_append(struct abc* yy, const char* yytext)
{
    struct abc_symbol* new = abc_new_symbol(yy);
    new->kind = ABC_EOL;
    new->text = strdup(yytext);
}
void abc_space_append(struct abc* yy, const char* yytext)
{
    struct abc_symbol* new = abc_new_symbol(yy);
    new->kind = ABC_SPACE;
    new->text = strdup(yytext);
}

void abc_note_append(struct abc* yy, const char* yytext)
{
    struct abc_symbol* new = abc_new_symbol(yy);

    new->kind = ABC_NOTE;
    new->dur_num = 1;
    new->dur_den = 1;
    new->text = strdup(yytext);
    new->ev.key = note2key(yy->ks, new->text, yy->measure_accid);
    if (new->text[0] == 'Z') {
        /* fix numerator */
        new->dur_num = abc_unit_per_measure(yy->ul, yy->mm);
    }
}

void abc_notepunct_set(struct abc* yy, const char* yytext)
{
    int count = strlen(yytext);

    struct abc_tune* tune = yy->tunes[yy->count-1];
    struct abc_voice* voice = tune->voices[tune->count-1];

    if (yytext[0] == '>') {
        voice->last->dur_den *= pow(2, count);

        int num, den;
        num = voice->last->prev->dur_num;
        den = voice->last->prev->dur_den;
        while (count) {
            abc_frac_add(&voice->last->prev->dur_num, &voice->last->prev->dur_den, num, den * pow(2, count));
            count--;
        }
    } else if (yytext[0] == '<') {
        voice->last->prev->dur_den *= pow(2, count);

        int num, den;
        num = voice->last->dur_num;
        den = voice->last->dur_den;
        while (count) {
            abc_frac_add(&voice->last->dur_num, &voice->last->dur_den, num, den * pow(2, count));
            count--;
        }
    }
}

void abc_grace_append(struct abc* yy, const char* yytext)
{
    struct abc_symbol* new = abc_new_symbol(yy);
    new->kind = ABC_GRACE;
    new->text = strdup(yytext);
}

void abc_chord_append(struct abc* yy, const char* yytext)
{
    struct abc_symbol* new = abc_new_symbol(yy);
    new->kind = ABC_CHORD;
    new->text = strdup(yytext);
    if (new->text[0] == ']') {
        struct abc_symbol* s = abc_chord_rewind(new->prev);
        while (s->next && s->next->text[0] != ']') {
            s = s->next;
            if (s->kind == ABC_NOTE)
                s->in_chord = 1;
        }
    }
}

int abc_chord_parse_num(const char* chord)
{
    int ret = 0;
    int num = 0;

    while (*chord) {
        if ('/' == *chord)
            return ret? ret: 1;

        if (isdigit(*chord)) {
            num = 1;
            ret *= 10;
            ret += *chord - '0';
        } else {
            num = 0;
        }

        if (ret && !num)
            return ret;

        chord++;
    }

    if (ret)
        return ret;

    return 1;
}

int abc_chord_parse_den(const char* chord)
{
    int den = 0;
    int ret = 0;

    while (*chord) {
        if (isalpha(*chord)) {
            if (ret && den)
                return ret;
        }

        if ('/' == *chord) {
            if (ret && den)
                return ret;

            den = !den;
        }

        if (den && isdigit(*chord)) {
            ret *= 10;
            ret += *chord - '0';
        }

        chord++;
    }

    if (ret)
        return ret;

    return 1;
}

void abc_duration_num_set(struct abc* yy, const char* yytext)
{
    int num = atoi(yytext);

    struct abc_tune* tune = yy->tunes[yy->count-1];
    struct abc_voice* voice = tune->voices[tune->count-1];

    voice->last->dur_num = num? num: 1;
    voice->last->dur_den = 1;
}

void abc_duration_den_set(struct abc* yy, const char* yytext)
{
    struct abc_tune* tune = yy->tunes[yy->count-1];
    struct abc_voice* voice = tune->voices[tune->count-1];

    if (yytext[0] == '/')
        /* divide by 2 as many as '/' */
        voice->last->dur_den = pow(2, strlen(yytext));
    else
        voice->last->dur_den = atoi(yytext);
}

int abc_is_endbar(const struct abc_symbol* s) {
    return (strstr(s->text, "||") || strstr(s->text, "|]"));
}

int abc_is_start(const struct abc_symbol* s) {
    return (strstr(s->text, "|:") != NULL);
}

int abc_is_repeat(const struct abc_symbol* s) {
    return (strstr(s->text, ":|") != NULL);
}

void abc_bar_append(struct abc* yy, const char* yytext)
{
    struct abc_symbol* new = abc_new_symbol(yy);
    new->kind = ABC_BAR;
    new->text = strdup(yytext);
    memset(yy->measure_accid, 0, sizeof(yy->measure_accid));

    struct abc_tune* t = yy->tunes[yy->count-1];
    struct abc_voice* v = t->voices[t->count-1];
    if (abc_is_endbar(new) || abc_is_repeat(new)) {
        v->in_alt = 0;
    }
    // for now, only bars and have 'in_alt' field set.
    new->in_alt = v->in_alt;
}

int abc_alt_is_of(const struct abc_symbol* s, int alt) {
    return (strchr(s->text, alt + '0') != NULL);
}

void abc_alt_append(struct abc* yy, const char* yytext)
{
    struct abc_symbol* new = abc_new_symbol(yy);
    new->kind = ABC_ALT;
    new->text = strdup(yytext);
    struct abc_tune* t = yy->tunes[yy->count-1];
    struct abc_voice* v = t->voices[t->count-1];
    v->in_alt = 1;
}

void abc_nuplet_append(struct abc* yy, int p, int q, int r)
{
    struct abc_symbol* new = abc_new_symbol(yy);
    new->kind = ABC_NUP;
    if (-1 == asprintf(&new->text, "%d:%d:%d", p, q, r)) {;}
}

void abc_deco_append(struct abc* yy, const char* yytext)
{
    struct abc_symbol* new = abc_new_symbol(yy);
    new->kind = ABC_DECO;
    new->text = strdup(yytext);
}

void abc_gchord_append(struct abc* yy, const char* yytext)
{
    struct abc_symbol* new = abc_new_symbol(yy);
    new->kind = ABC_GCHORD;
    new->text = strdup(yytext);
}

void abc_tie_append(struct abc* yy, const char* yytext)
{
    struct abc_symbol* new = abc_new_symbol(yy);
    new->kind = ABC_TIE;
    new->text = strdup(yytext);
    if (new->prev)
        new->prev->will_tie = 1;
}

void abc_slur_append(struct abc* yy, const char* yytext)
{
    struct abc_symbol* new = abc_new_symbol(yy);
    new->kind = ABC_SLUR;
    new->text = strdup(yytext);
}

void abc_delete_symbols(struct abc_symbol* s) {
    struct abc_symbol* next = s;

    while (s) {
        free(s->text);
        free(s->lyr);
        next = s->next;
        free(s);
        s = next;
    }
}

void abc_change_append(struct abc* yy, const char* yytext)
{
    struct abc_symbol* new = abc_new_symbol(yy);
    new->kind = ABC_CHANGE;
    new->text = strdup(yytext);
    new->ev.start_den = 1;
    if (new->text[0] == 'K') {
        yy->ks = &new->text[2];
        new->ev.type = EV_KEYSIG;
        int mode;
        new->ev.key = smf_key_signature(yy->ks, &mode);
        new->ev.value = mode;
    } else if (new->text[0] == 'Q') {
        new->ev.type = EV_TEMPO;
        new->ev.value = abc_tempo(&new->text[2]);
    } else if (new->text[0] == 'M') {
        new->ev.type = EV_METRIC;
        yy->mm = &new->text[2];
        if (2 != sscanf(yy->mm, "%d/%d", &new->ev.key, &new->ev.value))
            new->ev.key = 4, new->ev.value = 4;
    } else if (new->text[0] == 'L') {
        new->ev.type = EV_UNIT;
        yy->ul = &new->text[2];
        if (2 != sscanf(yy->ul, "%d/%d", &new->ev.key, &new->ev.value))
            new->ev.key = 1, new->ev.value = 8;
    }
}

struct abc* abc_alloc_yy(void)
{

    struct abc* yy = calloc(1, sizeof (*yy));
    yy->buffer = calloc(1, sizeof (*yy->buffer));
    return yy;
}

void abc_release_yy(struct abc* yy)
{
    free(yy->buffer->buf);
    free(yy->buffer);

    for (int i = 0; i < yy->count; i++) {
        struct abc_tune* t = yy->tunes[i];
        struct abc_header* h = t->headers;
        while (h) {
            struct abc_header* hn = h->next;
            free(h->text);
            free(h);
            h = hn;
        }

        for (int j = 0; j < t->count; j++) {
            struct abc_voice* v = t->voices[j];
            abc_release_voice(v);
        }

        free(t->voices);
        free(t);
    }

    free(yy->tunes);
    free(yy);
}

struct abc* abc_parse_buffer(const char* buffer, int size) {
    struct abc* yy = abc_alloc_yy();

    yy->buffer->buf = malloc(size * sizeof (char));
    yy->buffer->buf = memcpy(yy->buffer->buf, buffer, size);
    yy->buffer->count = size;

    pcc_context_t *ctx = pcc_create(yy);

    while (pcc_parse(ctx, NULL)) {
        if (yy->error){
#if 0
            FILE* out = fopen("out.abc", "w");
            fprintf(out, "%.*s", yy->buffer->count, yy->buffer->buf);
            fclose(out);
#endif
            break;
        }
    }

    pcc_destroy(ctx);
    return yy;
}

struct abc_tune* abc_find_tune(struct abc* yy, int x) {
    struct abc_tune* t = NULL;
    for (int i = 0; i < yy->count; i++) {
        if (yy->tunes[i]->x == x) {
            t = yy->tunes[i];
            break;
        }
    }

    return t;
}

struct abc_header* abc_find_header(const struct abc_tune* t, char h) {
    if (!t) return NULL;

    struct abc_header* header = t->headers;

    while(header) {
        if (header->h == h) {
            return header;
        }

        header = header->next;
    }

    return header;
}

/* find previous (including last) symbol that is a note or chord */
struct abc_symbol* abc_prev_note_or_chord(struct abc_symbol* s) {
    while (s) {
        if (s->kind == ABC_CHORD || s->kind == ABC_NOTE)
            return s;
        s = s->prev;
    }

    return s;
}

/* find next symbol that is a note or chord */
struct abc_symbol* abc_next_note_or_chord(struct abc_symbol* s) {
    while (s->next) {
        s = s->next;
        if (s->kind == ABC_CHORD || s->kind == ABC_NOTE)
            return s;
    }

    return s;
}

/* rewind and return the first symbol just after start repeat */
struct abc_symbol* abc_find_start_repeat(struct abc_symbol* s) {
    while (s->prev) {
        s = s->prev;
        if (s->kind == ABC_BAR && strstr(s->text, "|:"))
            return s->next;
    }

    return s;
}

/* return the next repeat bar */
struct abc_symbol* abc_find_next_repeat(struct abc_symbol* s) {
    while (s->next) {
        s = s->next;
        if (s->kind == ABC_BAR && abc_is_repeat(s))
            return s;
    }

    return s;
}

/* return a bar symbol matching next alternation in pass of alt */
struct abc_symbol* abc_find_next_alt(struct abc_symbol* s, int alt) {
    while (s->next) {
        s = s->next;
        /*
           if (s->kind == ABC_BAR && abc_is_repeat(s))
           return NULL;
           */
        if (s->kind == ABC_BAR && abc_is_endbar(s)) {
            return s;
        }

        if ((s->kind == ABC_ALT) && abc_alt_is_of(s, alt)) {
            return s->prev;
        }

        if (s->kind == ABC_BAR) {
            if (!s->in_alt)
                return s;
        }
    }

    return s;
}

struct abc_symbol* abc_find_next_segno(struct abc_symbol* s) {
    while (s->next) {
        s = s->next;
        if (s->kind == ABC_DECO && !strcmp(s->text, "segno"))
            return s;
    }

    return s;
}

int abc_has_pair(struct abc_symbol* s, int chord) {
    const char* note = s->text;
    if (chord) {
        int in = 0;
        s = abc_chord_forward(s);
        s = s->next; /* '-' */
        while (s->next && s->next->text[0] != ']') {
            s = s->next;
            if (!in && (s->kind == ABC_NOTE))
                return 0;

            if (in && (s->kind == ABC_NOTE)) {
                if (!strcmp(note, s->text))
                    return 1;
            }
            if (s->text[0] == '[')
                in = 1;
        }
    } else {
        while (s->next) {
            s = s->next; /* '-' */
            if (s->kind == ABC_NOTE) {
                if (!strcmp(note, s->text))
                    return 1;
                return 0;
            }
        }
    }

    return 0;
}

int abc_has_tie(struct abc_symbol* s, int chord) {
    if (s->will_tie)
        return 1;

    while (s->next) {
        s = s->next;
        if (chord) {
            if ((s->kind == ABC_CHORD) && s->text[0] == ']') {
                if (s->next && (s->next->kind == ABC_TIE))
                    return 1;
                return 0;
            }
        } else {
            if (s->kind == ABC_TIE)
                return 1;
            return 0;
        }
    }

    return 0;
}

double abc_grace_duration(struct abc_symbol* s) {
    if (s->kind != ABC_GRACE)
        return 0.0;

    double dur = 0.0;
    while (s->next) {
        s = s->next;
        if (s->kind == ABC_NOTE) {
            dur += (double) s->dur_num / (double) s->dur_den;
        }

        if (s->kind == ABC_GRACE) /* end of grace */
            return dur;
    }

    return dur;
}

double abc_apply_divide(const char* div) {
    if (!strchr(div, '/'))
        return 0.0;

    int num, den;
    if(2 != sscanf(div, "%d/%d", &num, &den))
        return 0.0;
    return (double) num / (double) den;
}

int abc_unit_per_measure(const char* l, const char* m) {
    if (!l)
        l = "1/8";
    if (!m)
        m = "4/4";

    int ln, ld, mn, md;
    if (2 == sscanf(l, "%d/%d", &ln, &ld)) {;}
    else { ln = 1; ld = 8; }

    if (2 == sscanf(m, "%d/%d", &mn, &md)) {;}
    else { mn = 4; md = 4; }

    return (ld * mn) / (md * ln);
}

/* tempo in quarter note per minute */
long abc_tempo(const char* t) {
    long tempo = 120;
    if (t) {
        int q = 0;
        int num, den;
        if (3 == sscanf(t, "%d/%d=%d", &num, &den, &q))
            tempo = (q * 4 * num) / den;
        else if (1 == sscanf(t, "%d", &q))
            tempo = q;
        else if (!strncasecmp(t, "\"Largo\"", 5))
            tempo = 40;
        else if (!strncasecmp(t, "\"Larghetto\"", 9))
            tempo = 60;
        else if (!strncasecmp(t, "\"Lento\"", 5))
            tempo = 50;
        else if (!strncasecmp(t, "\"Adagio\"", 6))
            tempo = 60;
        else if (!strncasecmp(t, "\"Andante\"", 7))
            tempo = 80;
        else if (!strncasecmp(t, "\"Moderato\"", 8))
            tempo = 90;
        else if (!strncasecmp(t, "\"Allegro\"", 7))
            tempo = 120;
        else if (!strncasecmp(t, "\"Presto\"", 6))
            tempo = 140;
        else if (!strncasecmp(t, "\"Vivace\"", 6))
            tempo = 160;
        else
            tempo = 120;
    }

    return tempo;
}

/* m must be the measure metric */
void abc_compute_pqr(int* p, int* q, int* r, const char* m) {
    int num, den;

    if (!m || m[0] == 'C')
        m = "4/4";

    if (2 != sscanf(m, "%d/%d", &num, &den))
        num = den = 4;

    if (!*r)
        *r = *p;

    if (!*q) {
        switch (*p) {
            case 2: *q = 3; break;
            case 3: *q = 2; break;
            case 4: *q = 3; break;
            case 5: if (!(num % 3) && (den == 8)) *q = 3; else *q = 2; break;
            case 6: *q = 2; break;
            case 7: if (!(num % 3) && (den == 8)) *q = 3; else *q = 2; break;
            case 8: *q = 3; break;
            case 9: if (!(num % 3) && (den == 8)) *q = 3; else *q = 2; break;
        }

    }
}

struct abc_symbol* abc_dup_symbol(struct abc_symbol* from) {
    struct abc_symbol* to = calloc(1, sizeof (struct abc_symbol));
    to = memcpy(to, from, sizeof (*from));
    to->prev = NULL;
    to->next = NULL;
    if (from->text)
        to->text = strdup(from->text);
    if (from->lyr)
        to->lyr = strdup(from->lyr);
    return to;
}

void abc_voice_append_symbol(struct abc_voice* voice, struct abc_symbol* s) {
    if (s) {
        if (voice->last == NULL) {
            voice->last = s;
            voice->first = voice->last;
        } else {
            struct abc_symbol* l = voice->last;
            voice->last = s;
            l->next = s;
            s->prev = l;
            s->index = l->index + 1;
        }
    }
}

void abc_frac_add(long* num, long* den, long from_num, long from_den) {
    long ppcm = *den > from_den ? *den : from_den;

    while (1) {
        if ((ppcm % *den) || (ppcm % from_den)) {
            ppcm++;
            continue;
        }
        break;
    }


    long cofactor1 = ppcm / *den;
    long cofactor2 = ppcm / from_den;

    long num1 = *num * cofactor1;
    long num2 = from_num * cofactor2;

    *num = num1 + num2;
    *den = ppcm;
}

void abc_duration_add(struct abc_symbol* to, struct abc_symbol* with) {
    long num1 = to->dur_num * with->dur_den;
    long num2 = with->dur_num * to->dur_den;
    long den = to->dur_den * with->dur_den;
    to->dur_num = num1 + num2;
    to->dur_den = den;
}

static int event_cmp(const void* a, const void* b) {
    const struct abc_symbol* s1 = a;
    const struct abc_symbol* s2 = b;
    long s1_v, s2_v;

    /* if the tick values are the same, return noteoff first.
     * this allow repeated notes to not overlap in some cases. */
    if (s1->ev.start_num == s2->ev.start_num
            && s1->ev.start_den == s2->ev.start_den) {
        return s1->ev.value - s2->ev.value;
    }

    s1_v = (double) s1->ev.start_num / (double) s1->ev.start_den;
    s2_v = (double) s2->ev.start_num / (double) s2->ev.start_den;

    if (s1_v > s2_v)
        return 1;
    if (s1_v < s2_v)
        return -1;

    return  0;
}

struct abc_voice* abc_pass3_ungroup_voice(const struct abc_voice* v) {
    int in_chord = 0;

    /* use absolute ticks */
    long tick_num = 0, tick_den = 1;

    struct abc_voice* voice = calloc(1, sizeof (struct abc_voice));
    voice->v = strdup(v->v);

    struct abc_symbol* s = v->first;
    while (s) {
        switch (s->kind) {
            case ABC_CHORD: {
                                if (s->text[0] == '[')
                                    in_chord = 1;
                                else
                                    in_chord = 0;
                            }
                            break;
            case ABC_NOTE: {
                               if (!in_chord) {
                                   struct abc_symbol* on = NULL;
                                   struct abc_symbol* off = NULL;

                                   on = abc_dup_symbol(s);
                                   on->ev.value = 1;

                                   abc_frac_add(&tick_num, &tick_den, on->dur_num, on->dur_den);

                                   off = abc_dup_symbol(on);
                                   off->ev.start_num = tick_num;
                                   off->ev.start_den = tick_den;
                                   off->dur_num = 0;
                                   off->dur_den = 1;
                                   off->ev.value = 0;

                                   abc_voice_append_symbol(voice, on);
                                   abc_voice_append_symbol(voice, off);
                               } else {
                                   /* prepare note on */
                                   struct abc_symbol* starts = NULL;
                                   int start_count = 0;
                                   while (s->kind != ABC_CHORD) { /* got to ']' */
                                       if (s->kind == ABC_NOTE) {
                                           starts = realloc(starts, ++start_count * sizeof (*starts));
                                           struct abc_symbol* dup = abc_dup_symbol(s);
                                           starts[start_count - 1] = *dup;
                                           starts[start_count - 1].ev.value = 1;

                                           free(dup);

                                       } else {
                                           /* if we have a BAR, insert it before chord */
                                           struct abc_symbol* new = NULL;
                                           new = abc_dup_symbol(s);
                                           abc_voice_append_symbol(voice, new);
                                       }
                                       s = s->next;
                                   }

                                   /* prepare note off based on previous array */
                                   struct abc_symbol* stops = NULL;
                                   int stop_count = start_count;
                                   stops = realloc(stops, stop_count * sizeof (*stops));
                                   for (int i = 0; i < stop_count; i++) {
                                       struct abc_symbol* dup = abc_dup_symbol(&starts[i]);
                                       stops[i] = *dup;
                                       stops[i].ev.value = 0;

                                       free(dup);
                                   }

                                   /* reorganize note off members */
                                   for (int i = 0; i < stop_count; i++) {
                                       struct abc_symbol* e = &stops[i];
                                       abc_frac_add(&e->ev.start_num, &e->ev.start_den, e->dur_num, e->dur_den);

                                       e->dur_num = 0;
                                       e->dur_den = 1;
                                   }

                                   struct abc_symbol* events = NULL;
                                   int event_count = start_count + stop_count;
                                   events = realloc(events, event_count * sizeof (*events));
                                   for (int i = 0; i < start_count; i++) {
                                       struct abc_symbol* dup = abc_dup_symbol(&starts[i]);
                                       events[i] = *dup;

                                       free(dup);
                                   }

                                   /* free garbage */
                                   for (int i = 0; i < start_count; i++) {
                                       free(starts[i].text);
                                       free(starts[i].lyr);
                                   }

                                   free(starts);

                                   for (int i = 0; i < stop_count; i++) {
                                       struct abc_symbol* dup = abc_dup_symbol(&stops[i]);
                                       events[i + start_count] = *dup;

                                       free(dup);
                                   }

                                   /* free garbage */
                                   for (int i = 0; i < stop_count; i++) {
                                       free(stops[i].text);
                                       free(stops[i].lyr);
                                   }

                                   free(stops);

                                   /* sort in time on/off events */
                                   qsort(events, event_count, sizeof (*events), event_cmp);

                                   for (int i = 0; i < event_count; i++) {
                                       abc_voice_append_symbol(voice, abc_dup_symbol(&events[i]));
                                   }

                                   /* we must use the complete chord duration as duration to add to main tick! */
                                   int first_start_num = events[0].ev.start_num;
                                   int first_start_den = events[0].ev.start_den;
                                   int last_stop_num = events[event_count -1].ev.start_num;
                                   int last_stop_den = events[event_count - 1].ev.start_den;
                                   abc_frac_add(&tick_num, &tick_den, last_stop_num, last_stop_den);
                                   abc_frac_add(&tick_num, &tick_den, -first_start_num, first_start_den);

                                   /* free garbage */
                                   for (int i = 0; i < event_count; i++) {
                                       free(events[i].text);
                                       free(events[i].lyr);
                                   }

                                   free(events);

                                   /* go just before ']' */
                                   s = s->prev;
                               }
                           }
                           break;
            default: {
                         /* and ABC_BAR and ABC_SPACE */
                         struct abc_symbol* new = NULL;
                         new = abc_dup_symbol(s);
                         abc_voice_append_symbol(voice, new);
                     }

        }

        s = s->next;
    }

    return voice;
}

#define GRACE_DEN 4
struct abc_untie_ctx {
    long tick_num, tick_den; /* absolute ticks */
    int nup_p, nup_q, nup_r; /* n-uplets */
    int in_chord;            /* chord state */
    int in_grace;            /* grace state */
    long grace_num, grace_den;
    long l_num, l_den, l_mul, l_div; /* L */
    const char* m;
    long chord_num, chord_den; /* chord duration */

    struct abc_symbol* ties[32]; /* per-note ties */
    int ties_len;

    struct abc_symbol* next_ties[32]; /* per-note ties */
    int next_ties_len;
    int prev_chord;
};

static struct abc_symbol* abc_untie_voice_produce_single_note(struct abc_untie_ctx* ctx, struct abc_symbol* s) {
    /* produce simple note */
    struct abc_symbol* new = abc_dup_symbol(s);
    new->dur_num *= ctx->l_mul;
    new->dur_den *= ctx->l_div;

    if (ctx->in_grace) {
        new->dur_den *= GRACE_DEN;
        abc_frac_add(&ctx->grace_num, &ctx->grace_den, new->dur_num, new->dur_den);
    } else {
        abc_frac_add(&new->dur_num, &new->dur_den, -ctx->grace_num, ctx->grace_den);
        ctx->grace_num = 0;
        ctx->grace_den = 1;
    }

    if (ctx->nup_r) {
        new->dur_num *= ctx->nup_q;
        new->dur_den *= ctx->nup_p;
        ctx->nup_r--;
    }

    abc_frac_add(&new->ev.start_num, &new->ev.start_den, ctx->tick_num, ctx->tick_den);
    abc_frac_add(&ctx->tick_num, &ctx->tick_den, new->dur_num, new->dur_den);

    return new;
}

static struct abc_symbol* abc_untie_voice_produce_single_chord(struct abc_untie_ctx* ctx, struct abc_symbol* s) {
    struct abc_symbol* new = abc_dup_symbol(s);
    new->dur_num *= ctx->l_mul;
    new->dur_den *= ctx->l_div;

    if (ctx->nup_r) {
        new->dur_num *= ctx->nup_q;
        new->dur_den *= ctx->nup_p;
    }

    abc_frac_add(&new->ev.start_num, &new->ev.start_den, ctx->tick_num, ctx->tick_den);
    /* we use the first note of chord as chord duration! */
    if (ctx->chord_num == 0) {
        ctx->chord_num = new->dur_num;
        ctx->chord_den = new->dur_den;
    }

    return new;
}

static void abc_untie_voice_lengthen_tied_note(struct abc_untie_ctx* ctx, struct abc_symbol* s) {
    struct abc_symbol* p = NULL;
    for (int i = 0; i < ctx->ties_len; i++) {
        /* compare MIDI pitch, not text (which could differ with alteration text) */
        if (ctx->ties[i]->ev.key == s->ev.key) {
            p = ctx->ties[i];
            break;
        }
    }

    int nup_num = ctx->l_mul * s->dur_num;
    int nup_den = ctx->l_div * s->dur_den;
    if (ctx->nup_r) {
        nup_num *= ctx->nup_q;
        nup_den *= ctx->nup_p;
        ctx->nup_r--;
    }

    if (p && p->kind == ABC_NOTE) {
        abc_frac_add(&p->dur_num, &p->dur_den, nup_num, nup_den);
        abc_frac_add(&ctx->tick_num, &ctx->tick_den, nup_num, nup_den);
    }
}

static void abc_untie_voice_lengthen_tied_chord(struct abc_untie_ctx* ctx, struct abc_symbol* s, struct abc_voice* voice) {
    int found = 0;
    for (int i = 0; i < ctx->ties_len; i++) {
        struct abc_symbol* p = ctx->ties[i];
        /* compare MIDI pitch, not text (which could differ with alteration text) */
        if (s->kind == ABC_NOTE && p->ev.key == s->ev.key) {
            found = 1;
            /* make tie */
            long dur_num = ctx->l_mul * s->dur_num;
            long dur_den = ctx->l_div * s->dur_den;

            if (ctx->nup_r) {
                dur_num *= ctx->nup_q;
                dur_den *= ctx->nup_p;
            }

            abc_frac_add(&p->dur_num, &p->dur_den, dur_num, dur_den);
            /* we use the first note of this chord to add to chord duration! */
            if (ctx->chord_num == 0) {
                abc_frac_add(&ctx->chord_num, &ctx->chord_den, dur_num, dur_den);
            }

            break;
        }
    }

    if (s->kind == ABC_NOTE && !found) {
        /* this note has to be created (no tie) */
        struct abc_symbol* n = abc_dup_symbol(s);
        n->dur_num *= ctx->l_mul;
        n->dur_den *= ctx->l_div;
        if (ctx->nup_r) {
            n->dur_num *= ctx->nup_q;
            n->dur_den *= ctx->nup_p;
        }
        abc_frac_add(&n->ev.start_num, &n->ev.start_den, ctx->tick_num, ctx->tick_den);
        abc_voice_append_symbol(voice, n);

        /* we use the first note of this chord to add to chord duration! */
        if (ctx->chord_num == 0) {
            abc_frac_add(&ctx->chord_num, &ctx->chord_den, n->dur_num, n->dur_den);
        }
    }
}

void update_ties(struct abc_untie_ctx* ctx) {
    for (int i = 0; i < ctx->next_ties_len; i++) {
        ctx->ties[i] = ctx->next_ties[i];
    }

    ctx->ties_len = ctx->next_ties_len;
    ctx->next_ties_len = 0;
}

void update_next_ties(struct abc_untie_ctx* ctx, struct abc_symbol* s, struct abc_voice* voice) {
    int found = 0;
    for (int i = 0; i < ctx->ties_len; i++) {
        if (s && ctx->ties[i]->ev.key == s->ev.key) {
            found = 1;
            ctx->next_ties[ctx->next_ties_len++] = ctx->ties[i];
        }
    }

    if (s && !found) {
        /* s produced note in voice->last */
        ctx->next_ties[ctx->next_ties_len++] = voice->last;
    }
}

void build_ties(struct abc_untie_ctx* ctx, struct abc_voice* voice, int prev_chord) {
    ctx->ties_len = 0;
    struct abc_symbol* p = voice->last;

    /* there can have spaces or bars when untying (pass 2) */
    while (p && (p->kind != ABC_CHORD && p->kind != ABC_NOTE))
        p = p->prev;

    /* there can be a note, or a chord just before the tie */
    if (p && (p->kind == ABC_CHORD || prev_chord)) {
        p = abc_chord_rewind(p->prev);
        p = p->next;
    }

    while (p && p->kind != ABC_TIE && p->kind != ABC_CHORD) {
        if (p->kind == ABC_NOTE) {
            ctx->ties[ctx->ties_len++] = p;
        }

        p = p->next;
    }
}

struct abc_voice* abc_pass2_untie_voice(struct abc_voice* v, struct abc_tune* t) {
    struct abc_untie_ctx ctx;
    memset(&ctx, 0, sizeof (ctx));
    ctx.tick_den = 1;
    ctx.grace_den = 1;
    ctx.l_den = 1;
    ctx.l_mul = 1;
    ctx.l_div = 1;
    ctx.chord_den = 1;

    struct abc_voice* voice = calloc(1, sizeof (struct abc_voice));
    voice->v = strdup(v->v);

    struct abc_header* mh = abc_find_header(t, 'M');
    if (!mh || mh->text[0] == 'C')
        ctx.m = "4/4";
    else
        ctx.m = mh->text;

    struct abc_header* h = abc_find_header(t, 'L');
    if (h && (2 == sscanf(h->text, "%ld/%ld", &ctx.l_num, &ctx.l_den))) {;}
    else {
        ctx.l_num = 1;
        ctx.l_den = 8;
    }

    struct abc_symbol* s = v->first;
    while (s) {
        struct abc_symbol* new = NULL;

        switch (s->kind) {
            case ABC_CHANGE: {
                                 if (s->text[0]== 'M') {
                                     ctx.m = &s->text[2];
                                 } else if (s->text[0]== 'L') {
                                     long l_n, l_d;
                                     if (2 == sscanf(&s->text[2], "%ld/%ld", &l_n, &l_d)) {
                                         ctx.l_mul = l_n / ctx.l_num;
                                         ctx.l_div = l_d / ctx.l_den;
                                     }
                                 } else {
                                     new = abc_dup_symbol(s);
                                 }
                             }
                             break;

            case ABC_NUP: {
                              if (3 == sscanf(s->text, "%d:%d:%d", &ctx.nup_p, &ctx.nup_q, &ctx.nup_r)) {
                                  abc_compute_pqr(&ctx.nup_p, &ctx.nup_q, &ctx.nup_r, ctx.m);
                              }
                          }
                          break;

            case ABC_GRACE: {
                                if (s->text[0] == '{') {
                                    ctx.in_grace = 1;
                                } else {
                                    ctx.in_grace = 0;
                                }
                            }
                            break;

            case ABC_CHORD: {
                                if (s->text[0] == '[') {
                                    ctx.in_chord = 1;
                                } else {
                                    update_ties(&ctx);
                                    ctx.in_chord = 0;
                                    abc_frac_add(&ctx.tick_num, &ctx.tick_den, ctx.chord_num, ctx.chord_den);
                                    ctx.chord_num = 0, ctx.chord_den = 1;
                                    if (ctx.nup_r)
                                        ctx.nup_r--;
                                    ctx.prev_chord = 1;
                                }

                                /* when ctx.ties_len > 0, the ']' symbol is not used, because the next one will be */

                                if (ctx.ties_len == 0 && !s->will_tie) {
                                    /* end of tied chord or not tied new chord */
                                    new = abc_dup_symbol(s);
                                }
                                else if (s->text[0] == ']' && s->will_tie && abc_next_note_or_chord(s)->kind == ABC_NOTE) {
                                    /* chord is tying and folowed by a single note and we were not tied */
                                    struct abc_symbol* p = abc_chord_rewind(s->prev);
                                    if (!p ->prev ||  p->prev->kind != ABC_TIE)
                                        /* and chord was not tied previously */
                                        new = abc_dup_symbol(s);
                                }
                                else if (s->text[0] == '[' && abc_prev_note_or_chord(voice->last)->kind == ABC_NOTE && !ctx.prev_chord) {
                                    /* prev note tied to chord: insert '[' symbol before last accepted note */
                                    struct abc_symbol* p = voice->last;
                                    while (p->kind != ABC_NOTE)
                                        p = p->prev;

                                    new = abc_dup_symbol(s);
                                    if (p && p->prev)
                                        p->prev->next = new;
                                    else /* p was first symbol */
                                        voice->first = new;
                                    if (p) {
                                        new->prev = p->prev;
                                        p->prev = new;
                                    }
                                    new->next = p;

                                    new = NULL;
                                }
                            }
                            break;

            case ABC_NOTE: {
                                if (!ctx.in_chord) {
                                    if (ctx.ties_len == 0) {
                                        new = abc_untie_voice_produce_single_note(&ctx, s);
                                    } else {
                                        if (ctx.prev_chord) {
                                            /* prev chord tied to this note: insert note before last accepted chord */
                                            abc_untie_voice_lengthen_tied_chord(&ctx, s, voice);
                                            abc_frac_add(&ctx.tick_num, &ctx.tick_den, ctx.chord_num, ctx.chord_den);
                                            ctx.chord_num = 0, ctx.chord_den = 1;
                                            if (ctx.nup_r)
                                                ctx.nup_r--;
                                        } else {
                                            abc_untie_voice_lengthen_tied_note(&ctx, s);
                                        }
                                        ctx.ties_len = 0;
                                    }
                                    ctx.prev_chord = 0;
                                } else {
                                    if (ctx.ties_len == 0)
                                        new = abc_untie_voice_produce_single_chord(&ctx, s);
                                    else
                                        abc_untie_voice_lengthen_tied_chord(&ctx, s, voice);
                                }
                            }
                            break;

            case ABC_TIE: {
                              if (ctx.in_chord) {
                                  update_next_ties(&ctx, s->prev, voice);
                              } else {
                                  build_ties(&ctx, voice, ctx.prev_chord);
                              }
                          }
                          break;

            default: {
                         /* and ABC_BAR and ABC_SPACE */
                         new = abc_dup_symbol(s);
                     }
        }

        if (new) {
            abc_voice_append_symbol(voice, new);
        }

        s = s->next;
    }
#if 0
    struct abc_symbol* y = voice->first;
    while (y) {
        fprintf(stderr, "%s: %ld/%ld-%ld/%ld\n", y->text, y->ev.start_num, y->ev.start_den, y->dur_num, y->dur_den);
        y = y->next;
    }
    exit(0);
#endif
    return voice;
}

struct abc_voice* abc_pass1_unfold_voice(struct abc_voice* v) {
    int pass = 1;
    struct abc_symbol* cur_repeat = NULL;

    struct abc_voice* voice = calloc(1, sizeof (struct abc_voice));
    voice->v = strdup(v->v);

    struct abc_symbol* s = v->first;
    while (s) {
        struct abc_symbol* new = NULL;

        switch (s->kind) {
            case ABC_BAR: {
                              new = calloc(1, sizeof (struct abc_symbol));
                              new->kind = ABC_BAR;
                              new->text = strdup("|");
                              new->ev.start_den = 1;
                              new->dur_den = 1;

                              if (abc_is_repeat(s)) {
                                  if (cur_repeat == s) {
                                      /* never reset cur_repeat to NULL (loops forever) */

                                      if (abc_is_start(s)) {
                                          pass = 1;
                                      }
                                  } else if (!cur_repeat || (cur_repeat->index < s->index)) {
                                      cur_repeat = s;
                                      s = abc_find_start_repeat(s);
                                      abc_voice_append_symbol(voice, new);
                                      pass++;
                                      continue;
                                  }
                              } else if (abc_is_start(s)) {
                                  pass = 1;
                              }
                              break;
                          }
            case ABC_ALT: {
                              new = NULL;

                              if (!abc_alt_is_of(s, pass)) {
                                  struct abc_symbol* n;
                                  if ((n = abc_find_next_alt(s, pass))) {
                                      s = n; /* s is a BAR */
                                      continue;
                                  }
                              }

                              break;
                          }
            default: {
                         new = abc_dup_symbol(s);
                     }
        }

        if (new) {
            abc_voice_append_symbol(voice, new);
        }
        s = s->next;
    }

    return voice;
}

struct abc_symbol* abc_chord_first_note(struct abc_symbol* s) {
    if (s->kind == ABC_CHORD) {
        while (s->next) {
            s = s->next;
            if (s->kind == ABC_NOTE)
                return s;
        }
    }

    return NULL;
}

struct abc_symbol* abc_chord_rewind(struct abc_symbol* s) {
    while (s) {
        if (s->kind == ABC_CHORD && s->text[0] == ']')
            return NULL;

        if ((s->kind == ABC_CHORD) && (s->text[0] == '['))
            return s;
        s = s->prev;
    }

    return NULL;
}

struct abc_symbol* abc_chord_forward(struct abc_symbol* s) {
    while (s) {
        if ((s->kind == ABC_CHORD) && (s->text[0] == ']'))
            return s;
        s = s->next;
    }

    return NULL;
}

void abc_release_voice(struct abc_voice* v) {
    struct abc_symbol* s = v->first;
    while (s != v->last) {
        struct abc_symbol* sn = s->next;
        free(s->lyr);
        free(s->text);
        free(s);
        s = sn;
    }

    if (v->last) {
        free(v->last->lyr);
        free(v->last->text);
        free(v->last);
    }

    free(v->v);
    free(v);
}
