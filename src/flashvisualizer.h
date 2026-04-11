#ifndef FLASHVISUALIZER_H
#define FLASHVISUALIZER_H

#include <stdio.h>
#include "flashlayout.h"

#define FLASHVIS_BAR_WIDTH 40

/**
 * Print a text-based bar chart of flash region usage to the given stream.
 * Each region is shown as a filled bar proportional to its size relative
 * to the total flash size.
 */
void flashvis_print_bar(const FlashLayout *layout, FILE *stream);

/**
 * Print a summary table (name, start, size, used%) of all regions.
 */
void flashvis_print_summary(const FlashLayout *layout, FILE *stream);

#endif /* FLASHVISUALIZER_H */
