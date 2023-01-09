#pragma once

void score__reset(void);
void score__update(void);
void score__increase(int number);
void score__decrease(int number);
void score__updating(void *p);
int score__return_score(void);