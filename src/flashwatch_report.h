/*
 * flashwatch_report.h - Reporting for flash watch results
 */

#ifndef FLASHWATCH_REPORT_H
#define FLASHWATCH_REPORT_H

#include "flashwatch.h"
#include <stdio.h>

void flashwatch_report_print(const FlashWatch *watch, FILE *out);
void flashwatch_report_triggered(const FlashWatch *watch, FILE *out);

#endif /* FLASHWATCH_REPORT_H */
