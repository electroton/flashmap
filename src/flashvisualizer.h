#ifndef FLASHVISUALIZER_H
#define FLASHVISUALIZER_H

#include <stdio.h>
#include "flashlayout.h"

#define FLASHVIS_BAR_WIDTH 40

/**
 * Print a text-based bar chart of flash region usage to the given stream.
 * Each region is shown as a filled bar proportional to its size relative
 * to the total flash size. Regions are printed in order of their start
 * address. The bar uses '#' characters for used space and '-' for free.
 *
 * @param layout  Pointer to the flash layout to visualize. Must not be NULL.
 * @param stream  Output stream to write to (e.g. stdout or stderr).
 */
void flashvis_print_bar(const FlashLayout *layout, FILE *stream);

/**
 * Print a summary table (name, start, size, used%) of all regions.
 *
 * @param layout  Pointer to the flash layout to summarize. Must not be NULL.
 * @param stream  Output stream to write to (e.g. stdout or stderr).
 */
void flashvis_print_summary(const FlashLayout *layout, FILE *stream);

/**
 * Print a single-line overview showing total flash size, total bytes
 * consumed by all regions, and the percentage of flash that is mapped.
 *
 * Example output:
 *   Flash usage: 327680 / 524288 bytes (62.5% mapped)
 *
 * @param layout  Pointer to the flash layout to report on. Must not be NULL.
 * @param stream  Output stream to write to (e.g. stdout or stderr).
 */
void flashvis_print_usage(const FlashLayout *layout, FILE *stream);

#endif /* FLASHVISUALIZER_H */
