#include "abc.h"

int main() {
	int c;
	char* buf = NULL;
	int count = 0;

	/* read stdin */
	while (EOF != (c = getc(stdin))) {
		count++;
		buf = (char*) realloc(buf, count);
		buf[count -1] = c;
	}
	
	struct abc* yy = abc_parse_buffer(buf, count);
	if (yy->error)
		puts("Parse error");

	printf("tunes: %d\n", yy->count);
	for (int i = 0; i < yy->count; i++) {
		struct abc_tune* t = yy->tunes[i];
		printf("tune %d\n", t->x);

		struct abc_header* h = t->headers;
		if (h)
			while (h && h->h != 'T')
				h = h->next;

		printf("title: %s\n", h? h->text: "");
		for (int j = 0; j < t->count; j++) {
			struct abc_voice* u = abc_pass1_unfold_voice(t->voices[j]);
			struct abc_voice* u2 = abc_pass2_0_untie_fix_voice(u);
			abc_release_voice(u);
			struct abc_voice* u3 = abc_pass2_1_untie_voice(u2, t);
			abc_release_voice(u2);
			struct abc_voice* v = abc_pass3_ungroup_voice(u3);
			abc_release_voice(u3);
			//struct abc_voice* v = t->voices[j];
			printf("tune %d, voice %s\n", t->x, v->v);
			struct abc_symbol* s;
			for (s = v->first; s; s = s->next) {
				printf("kind %d: key %d, start %ld/%ld -> duration %ld/%ld ", s->kind, s->ev.key, s->ev.start_num, s->ev.start_den, s->dur_num, s->dur_den);
				if (s->text) puts(s->text);
				if (s->lyr) puts(s->lyr);
			}

			abc_release_voice(v);
		}
	}

	if (yy->error)
		goto end;

end:
	abc_release_yy(yy);
	free(buf);
    return 0;
}
