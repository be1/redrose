#include <stdlib.h>
#include <stdio.h>
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
	if (yy->error) {
		printf("Parse error line %d, char %d\n", yy->error_line_nr, yy->error_char_nr);
        return 1;
    }

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
			struct abc_voice* v = abc_make_events_for_voice(t, j);
			//struct abc_voice* v = t->voices[j];
			printf("tune %d, voice %s\n", t->x, v->v);
			struct abc_symbol* s;
			for (s = v->first; s; s = s->next) {
				printf("kind %d: key %d, val: %d, start %ld/%ld -> duration %ld/%ld ", s->kind, s->ev.key, s->ev.value, s->ev.start_num, s->ev.start_den, s->dur_num, s->dur_den);
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
