void posix_process_lb(bool matched, token** tok, int* line_number, buffers* bufs, flags* fl, void* labels, FILE* in, FILE* out) {
    if (!matched)
        flag_ignore++;
}

void posix_process_rb(bool matched, token** tok, int* line_number, buffers* bufs, flags* fl, void* labels, FILE* in, FILE* out) {
    if (!matched)
        flag_ignore--;
}

void posix_process_a(bool matched, token** tok, int* line_number, buffers* bufs, flags* fl, void* labels, FILE* in, FILE* out) {
    if (!matched)
        return;
    char* arg = *((*tok)->args);
    strcat(bufs->append, "\n");
    strcat(bufs->append, (const char*) arg);
}

void posix_process_b(bool matched, token** tok, int* line_number, buffers* bufs, flags* fl, void* labels, FILE* in, FILE* out) {
    if (!matched)
        return;
    char* arg = *((*tok)->args);
    int i = 0;
    label* lbls = (label*) labels;
    while (lbls[i].key) {
        if (strcmp(lbls[i].key, arg) == 0) {
            *tok = lbls[i].target;
            break;
        }
        i++;
    }
}

void posix_process_c(bool matched, token** tok, int* line_number, buffers* bufs, flags* fl, void* labels, FILE* in, FILE* out) {
    if (!matched)
        return;
    char* arg = *((*tok)->args);
    strcpy(bufs->pattern, (const char*) arg);
    fl->nextcycle = true;
}

void posix_process_d(bool matched, token** tok, int* line_number, buffers* bufs, flags* fl, void* labels, FILE* in, FILE* out) {
    if (!matched)
        return;
    *(bufs->pattern) = 0;
    fl->nextcycle = true;
    fl->noprint = true;
}

void posix_process_D(bool matched, token** tok, int* line_number, buffers* bufs, flags* fl, void* labels, FILE* in, FILE* out) {
    if (!matched)
        return;
    char *new_pattern, *first_line = strtok_r(bufs->pattern, "\n", &new_pattern);
    if (!first_line)
        posix_process_d(matched, tok, line_number, bufs, fl, labels, in, out);
    else {
        strcpy(bufs->pattern, (const char*) new_pattern);
        fl->nextcycle = true;
        fl->noprint = true;
        fl->noread = true;
    }
}

void posix_process_g(bool matched, token** tok, int* line_number, buffers* bufs, flags* fl, void* labels, FILE* in, FILE* out) {
    if (!matched)
        return;
    strcpy(bufs->pattern, (const char*) bufs->hold);
}

void posix_process_G(bool matched, token** tok, int* line_number, buffers* bufs, flags* fl, void* labels, FILE* in, FILE* out) {
    if (!matched)
        return;
    strcat(bufs->pattern, "\n");
    strcat(bufs->pattern, (const char*) bufs->hold);
}

void posix_process_h(bool matched, token** tok, int* line_number, buffers* bufs, flags* fl, void* labels, FILE* in, FILE* out) {
    if (!matched)
        return;
    strcpy(bufs->hold, (const char*) bufs->pattern);
}

void posix_process_H(bool matched, token** tok, int* line_number, buffers* bufs, flags* fl, void* labels, FILE* in, FILE* out) {
    if (!matched)
        return;
    strcat(bufs->hold, "\n");
    strcat(bufs->hold, (const char*) bufs->pattern);
}

void posix_process_i(bool matched, token** tok, int* line_number, buffers* bufs, flags* fl, void* labels, FILE* in, FILE* out) {
    if (!matched)
        return;
    char* arg = *((*tok)->args);
    fputs((const char*)arg, out);
    fputc('\n', out);
}

void posix_process_l(bool matched, token** tok, int* line_number, buffers* bufs, flags* fl, void* labels, FILE* in, FILE* out) {
    if (!matched)
        return;
    printf("TODO %c\n", (*tok)->command);//TODO
}

void posix_process_n(bool matched, token** tok, int* line_number, buffers* bufs, flags* fl, void* labels, FILE* in, FILE* out) {
    if (!matched)
        return;
    fputs((const char*)bufs->pattern, out);
    fputc('\n', out);
    strcpy(bufs->pattern, (const char*) bufs->lookahead);
    if (!(bufs->lookahead)) {
        fl->stop = true;
        return;
    }
    get_new_line(bufs->lookahead, in);
    (*line_number)++;
}

void posix_process_N(bool matched, token** tok, int* line_number, buffers* bufs, flags* fl, void* labels, FILE* in, FILE* out) {
    if (!matched)
        return;
    if (!(bufs->lookahead)) {
        fl->stop = true;
        return;
    }
    bool ok = (1 + strlen(bufs->pattern) + strlen(bufs->lookahead) < BUFSIZE);
    if (ok) {
        strncat(bufs->pattern, "\n", 1);
        strncat(bufs->pattern, (const char*) bufs->lookahead, BUFSIZE-strlen(bufs->pattern));
    }
    get_new_line(bufs->lookahead, in);
    (*line_number)++;
    assert(ok, "Buffer overflow\n", );
}

void posix_process_p(bool matched, token** tok, int* line_number, buffers* bufs, flags* fl, void* labels, FILE* in, FILE* out) {
    if (!matched)
        return;
    fputs((const char*)bufs->pattern, out);
    fputc('\n', out);
}

void posix_process_P(bool matched, token** tok, int* line_number, buffers* bufs, flags* fl, void* labels, FILE* in, FILE* out) {
    if (!matched)
        return;
    char first_line[BUFSIZE];
    strcpy(first_line, (const char *) bufs->pattern);
    if (!strtok(first_line, "\n"))
        posix_process_p(matched, tok, line_number, bufs, fl, labels, in, out);
    else {
        fputs((const char*)first_line, out);
        fputc('\n', out);
    }
}

void posix_process_q(bool matched, token** tok, int* line_number, buffers* bufs, flags* fl, void* labels, FILE* in, FILE* out) {
    if (!matched)
        return;
    fl->stop = true;
}

void posix_process_r(bool matched, token** tok, int* line_number, buffers* bufs, flags* fl, void* labels, FILE* in, FILE* out) {
    if (!matched)
        return;
    char* arg = *((*tok)->args);
    FILE* file = fopen(arg, "r");
    if (!file)
        return;
    char buffer[BUFSIZE];
    while (fgets(buffer, BUFSIZE, file))
        fputs(buffer, out);
}

void posix_process_s(bool matched, token** tok, int* line_number, buffers* bufs, flags* fl, void* labels, FILE* in, FILE* out) {
    if (!matched)
        return;
    //char* arg = *((*tok)->args);
    printf("TODO %c\n", (*tok)->command);//TODO
}

void posix_process_t(bool matched, token** tok, int* line_number, buffers* bufs, flags* fl, void* labels, FILE* in, FILE* out) {
    if (!matched)
        return;
    //char* arg = *((*tok)->args);
    printf("TODO %c\n", (*tok)->command);//TODO
}

void posix_process_w(bool matched, token** tok, int* line_number, buffers* bufs, flags* fl, void* labels, FILE* in, FILE* out) {
    if (!matched)
        return;
    char* arg = *((*tok)->args);
    FILE* file = fopen(arg, "a");
    if (!file)
        return;
    fputc('\n', file);
    fputs(bufs->pattern, file);
}

void posix_process_x(bool matched, token** tok, int* line_number, buffers* bufs, flags* fl, void* labels, FILE* in, FILE* out) {
    if (!matched)
        return;
    char tmp[BUFSIZE];
    strcpy(tmp, (const char*) bufs->pattern);
    strcpy(bufs->pattern, (const char*) bufs->hold);
    strcpy(bufs->hold, (const char*) tmp);
}

void posix_process_y(bool matched, token** tok, int* line_number, buffers* bufs, flags* fl, void* labels, FILE* in, FILE* out) {
    if (!matched)
        return;
    char* old = ((*tok)->args)[0];
    char* new = ((*tok)->args)[1];
    if (!bufs->pattern)
        return;
    for (;*old;old++) {
        for (char* pattern = bufs->pattern; *pattern; pattern++)
            if (*pattern == *old)
                *pattern = *new;
        new++;
    }
}

void posix_process_eq(bool matched, token** tok, int* line_number, buffers* bufs, flags* fl, void* labels, FILE* in, FILE* out) {
    if (!matched)
        return;
    fprintf(out, "%d\n", *line_number);
}
