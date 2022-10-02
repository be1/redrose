#ifndef ABC_H
#define ABC_H

#include <ctype.h>	/* isdigit */
#include <stdlib.h>	/* atoi */
#include <stdio.h>	/* asprintf */

#ifdef __cplusplus
extern "C" {
#endif

enum abc_type { ABC_EOL, ABC_SPACE, ABC_NOTE, ABC_NUP, ABC_GRACE, ABC_CHORD, ABC_DECO, ABC_GCHORD, ABC_TIE, ABC_SLUR, ABC_BAR, ABC_ALT, ABC_INST, ABC_CHANGE };

struct abc_buffer {
	char* buf;
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
    char* ks; /* current key signature */
    char* ul;  /* current unit length */
    char* mm;  /* current measure metric */
    int measure_accid['h']; /* 'g' + 1 */
};

struct abc_header {
	char h;
	char* text;
	struct abc_header* next;
};

struct abc_tune {
	int x;
	struct abc_header* headers;
    struct abc_voice** voices;
	int count;
};

struct abc_voice {
	char* v;
	struct abc_symbol* first;
	struct abc_symbol* last;
	int in_alt;
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
	enum abc_type kind;
	char* lyr;
	char* text;
	long dur_num; /* duration */
	long dur_den;
	int index; /* symbol index in parser */
    struct abc_event ev;

	int in_alt;
	int in_tie;
    int in_chord;

	struct abc_symbol* next;
	struct abc_symbol* prev;
};

void abc_tune_append(struct abc* yy, const char* yytext);

void abc_header_append(struct abc* yy, const char* yytext, const char which);

void abc_voice_append(struct abc* yy, const char* yytext);

struct abc_symbol* abc_last_symbol(struct abc* yy);

struct abc_symbol* abc_new_symbol(struct abc* yy);

void abc_instruction_append(struct abc* yy, const char* yytext);

void abc_lyrics_append(struct abc* yy, const char* yytext);

void abc_eol_append(struct abc* yy, const char* yytext);

void abc_space_append(struct abc* yy, const char* yytext);

void abc_note_append(struct abc* yy, const char* yytext);

void abc_frac_add(long* num, long* den, long from_num, long from_den);

void abc_notepunct_set(struct abc* yy, const char* yytext);

void abc_grace_append(struct abc* yy, const char* yytext);

void abc_chord_append(struct abc* yy, const char* yytext);

int abc_chord_parse_num(const char* chord);

int abc_chord_parse_den(const char* chord);

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

void abc_delete_symbols(struct abc_symbol* s);

struct abc* abc_alloc_yy(void);

void abc_release_yy(struct abc* yy);

int abc_alt_is_of(const struct abc_symbol* s, int alt);

int abc_is_start(const struct abc_symbol* s);

int abc_is_repeat(const struct abc_symbol* s);

int abc_is_endbar(const struct abc_symbol* s);

struct abc_tune* abc_find_tune(struct abc* yy, int x);

struct abc_header* abc_find_header(const struct abc_tune* t, char h);

struct abc_symbol* abc_find_start_repeat(struct abc_symbol* s);

struct abc_symbol* abc_find_next_segno(struct abc_symbol* s);

struct abc_symbol* abc_find_next_alt(struct abc_symbol* s, int alt);

struct abc_symbol* abc_find_next_repeat(struct abc_symbol* s);

int abc_has_pair(struct abc_symbol* s, int chord);

int abc_has_tie(struct abc_symbol* s, int chord);

double abc_grace_duration(struct abc_symbol* s);

int abc_unit_per_measure(const char* l, const char* m);

double abc_time_per_unit(struct abc_tune* t, long base);

long abc_tempo(const char* t);

void abc_compute_pqr(int* p, int* q, int* r, const char* m);

struct abc* abc_parse_buffer(const char* buffer, int size);

void abc_duration_add(struct abc_symbol* to, struct abc_symbol* with);

struct abc_voice* abc_pass3_ungroup_voice(const struct abc_voice* v);

struct abc_voice* abc_pass2_untie_voice(struct abc_voice* v, struct abc_tune* t);

struct abc_voice* abc_pass1_unfold_voice(struct abc_voice* v);

struct abc_symbol* abc_chord_first_note(struct abc_symbol* s);

struct abc_symbol* abc_chord_rewind(struct abc_symbol* s);

struct abc_symbol* abc_chord_forward(struct abc_symbol* s);

void abc_release_voice(struct abc_voice* v);

struct abc_symbol* abc_dup_symbol(struct abc_symbol* from);

#ifdef __cplusplus
}
#endif

#endif
