#ifndef ABC_H
#define ABC_H

#ifdef __cplusplus
extern "C" {
#endif

enum abc_type { ABC_EOL, ABC_SPACE, ABC_NOTE, ABC_NUP, ABC_GRACE, ABC_CHORD, ABC_DECO, ABC_GCHORD, ABC_TIE, ABC_SLUR, ABC_BAR, ABC_ALT, ABC_INST, ABC_CHANGE };

struct abc_buffer {
    unsigned char* buf;
    int count;

    int index;
};

struct abc {
    struct abc_tune** tunes;
    int count;

    struct abc_buffer* buffer;

    int error;
    int error_line;
    int error_char;

    char* ks; /* first key signature */
    char* ul; /* first unit length */
    char* mm; /* first measure metric */
    char* qq; /* first measure tempo */
};

struct abc_header {
    char h;
    char* text;
    struct abc_header* next;
};

struct abc_tune {
    struct abc* abc; /* parent */
    int x;
    struct abc_header* headers;

    struct abc_voice** voices;
    int count;

    char lbc; /* linebreak char (usually '$') */
};

struct abc_voice {
    struct abc_tune* tune; /* parent */
    char* v;
    struct abc_symbol* first;
    struct abc_symbol* last;
    int measure;
    int in_alt;
    int measure_accid['h']; /* 'g' + 1 */
    char* i_ks; /* initial key signature */
    char* i_ul; /* initial unit length */
    char* i_mm; /* initial measure metric */
    char* i_qq; /* initial measure tempo */
    char* ks;  /* current key signature */
    char* ul;  /* current unit length */
    char* mm;  /* current measure metric */
    char* qq;  /* current measure tempo */
};

enum abc_event_type { EV_NOTE, EV_KEYSIG, EV_TEMPO, EV_METRIC, EV_UNIT };

struct abc_event {
    long start_num;
    long start_den;
    int key; /* note key, or keysignature sharps/flags numbers */
    int value; /* note velocity, or kesysignature maj/min mode, or tempo in quarter per measure */
    enum abc_event_type type;
};

struct abc_symbol {
    struct abc_voice* voice; /* parent */
    enum abc_type kind;
    char* lyr;
    char* text;
    long dur_num; /* duration */
    long dur_den;
    long measure; /* measure nr. */
    int index; /* symbol index in all parsed symbols of voice */
    int cidx; /* symbol's character index in buffer */
    struct abc_event ev;

    int in_alt;
    int will_tie;
    struct abc_symbol* of_chord; /* first note of chord of this note, or NULL */

    struct abc_symbol* next;
    struct abc_symbol* prev;
};

/* internal parser API */
void abc_tune_append(struct abc* yy, const char* yytext);

void abc_header_append(struct abc* yy, const char* yytext, const char which);

void abc_voice_append(struct abc* yy, const char* yytext);

struct abc_symbol* abc_voice_last_note(struct abc_voice* v);

struct abc_symbol* abc_note_before(struct abc_symbol* s);

struct abc_symbol* abc_last_symbol(struct abc* yy);

struct abc_symbol* abc_new_symbol(struct abc* yy);

void abc_instruction_append(struct abc* yy, const char* yytext);

void abc_lyrics_append(struct abc* yy, const char* yytext);

void abc_eol_append(struct abc* yy, const char* yytext);

void abc_space_append(struct abc* yy, const char* yytext);

void abc_note_append(struct abc* yy, const char* yytext, int pos);

void abc_frac_add(long* num, long* den, long from_num, long from_den);

void abc_chordpunct_set(struct abc* yy, const char* yytext);

void abc_notepunct_set(struct abc* yy, const char* yytext);

void abc_grace_append(struct abc* yy, const char* yytext);

void abc_chord_append(struct abc* yy, const char* yytext);

int abc_chord_parse_num(const char* chord);

int abc_chord_parse_den(const char* chord);

/* go to the '[' symbol of a chord */
struct abc_symbol* abc_chord_rewind(struct abc_symbol* s);

/* go to the ']' symbol of a chord */
struct abc_symbol* abc_chord_forward(struct abc_symbol* s);

void abc_duration_chord_num_set(struct abc* yy, const char* yytext);

void abc_duration_chord_den_set(struct abc* yy, const char* yytext);

void abc_duration_num_set(struct abc* yy, const char* yytext);

void abc_duration_den_set(struct abc* yy, const char* yytext);

void abc_bar_append(struct abc* yy, const char* yytext);

void abc_alt_append(struct abc* yy, const char* yytext);

void abc_nuplet_append(struct abc* yy, int p, int q, int r);

void abc_deco_append(struct abc* yy, const char* yytext);

void abc_gchord_append(struct abc* yy, const char* yytext);

void abc_tie_append(struct abc* yy, const char* yytext);

void abc_slur_append(struct abc* yy, const char* yytext);

void abc_change_append(struct abc* yy, const char* yytext);

/* public API */

/* parse ABC text buffer */
struct abc* abc_parse_buffer(const char* buffer, int size);

/* free parsed abc from memory */
void abc_release_yy(struct abc* yy);

/* make MIDI-like events from an ABC voice */
struct abc_voice* abc_make_events_for_voice(const struct abc_tune* t, int voice);

/* free voice from memory */
void abc_release_voice(struct abc_voice* v);

/* find tune nr 'x' in parsed abc 'yy' */
struct abc_tune* abc_find_tune(const struct abc* yy, int x);

/* find header 'h' in tine 't' */
struct abc_header* abc_find_header(const struct abc_tune* t, char h);

/* find the previous inline change */
struct abc_symbol* abc_find_previous_change(struct abc_symbol *s, char c);

/* gives the tempo from T header 't'*/
long abc_tempo(const char* th_text);

/* gives the nr of base units (in L header) per measure (based on M header) */
int abc_unit_per_measure(const char* lh_text, const char* mh_text);

void abc_debug_headers(struct abc* yy);

#ifdef __cplusplus
}
#endif

#endif
