#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "../src/flashsymbol.h"

static int tests_run    = 0;
static int tests_passed = 0;

#define TEST(name) do { tests_run++; printf("  [ ] %s ... ", name); } while (0)
#define PASS()     do { tests_passed++; printf("PASS\n"); } while (0)
#define FAIL(msg)  do { printf("FAIL: %s\n", msg); } while (0)

static void test_init(void)
{
    TEST("table initializes empty");
    FlashSymbolTable t;
    flash_symbol_table_init(&t);
    assert(flash_symbol_count(&t) == 0);
    PASS();
}

static void test_add_and_count(void)
{
    TEST("add symbols and count");
    FlashSymbolTable t;
    flash_symbol_table_init(&t);
    assert(flash_symbol_add(&t, "main", 0x08000100, 64, FLASH_SYMBOL_FUNCTION) == 0);
    assert(flash_symbol_add(&t, "g_buf", 0x20000000, 256, FLASH_SYMBOL_OBJECT) == 0);
    assert(flash_symbol_count(&t) == 2);
    PASS();
}

static void test_find_by_name(void)
{
    TEST("find symbol by name");
    FlashSymbolTable t;
    flash_symbol_table_init(&t);
    flash_symbol_add(&t, "isr_vector", 0x08000000, 192, FLASH_SYMBOL_SECTION);
    flash_symbol_add(&t, "main", 0x080000C0, 128, FLASH_SYMBOL_FUNCTION);

    const FlashSymbol *s = flash_symbol_find_by_name(&t, "main");
    assert(s != NULL);
    assert(s->address == 0x080000C0);
    assert(s->size == 128);
    assert(s->type == FLASH_SYMBOL_FUNCTION);

    assert(flash_symbol_find_by_name(&t, "nonexistent") == NULL);
    PASS();
}

static void test_find_by_address(void)
{
    TEST("find symbol by address");
    FlashSymbolTable t;
    flash_symbol_table_init(&t);
    flash_symbol_add(&t, "foo", 0x08001000, 32, FLASH_SYMBOL_FUNCTION);

    const FlashSymbol *s = flash_symbol_find_by_address(&t, 0x08001000);
    assert(s != NULL);
    assert(strncmp(s->name, "foo", FLASH_SYMBOL_NAME_MAX) == 0);

    assert(flash_symbol_find_by_address(&t, 0xDEADBEEF) == NULL);
    PASS();
}

static void test_add_invalid(void)
{
    TEST("add with invalid args returns -1");
    FlashSymbolTable t;
    flash_symbol_table_init(&t);
    assert(flash_symbol_add(NULL, "sym", 0, 0, FLASH_SYMBOL_UNKNOWN) == -1);
    assert(flash_symbol_add(&t, NULL, 0, 0, FLASH_SYMBOL_UNKNOWN) == -1);
    assert(flash_symbol_add(&t, "", 0, 0, FLASH_SYMBOL_UNKNOWN) == -1);
    PASS();
}

static void test_clear(void)
{
    TEST("clear resets table");
    FlashSymbolTable t;
    flash_symbol_table_init(&t);
    flash_symbol_add(&t, "a", 0x100, 4, FLASH_SYMBOL_OBJECT);
    flash_symbol_add(&t, "b", 0x200, 8, FLASH_SYMBOL_OBJECT);
    flash_symbol_table_clear(&t);
    assert(flash_symbol_count(&t) == 0);
    assert(flash_symbol_find_by_name(&t, "a") == NULL);
    PASS();
}

int main(void)
{
    printf("Running flashsymbol tests...\n");
    test_init();
    test_add_and_count();
    test_find_by_name();
    test_find_by_address();
    test_add_invalid();
    test_clear();
    printf("\nResults: %d/%d passed\n", tests_passed, tests_run);
    return (tests_passed == tests_run) ? 0 : 1;
}
