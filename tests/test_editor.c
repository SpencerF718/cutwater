#include "editor.h"
#include "buffer.h"
#include <assert.h>
#include <stdio.h>

#define INITIAL_CAPACITY 16

static void fill_buffer(EditorBuffer *eb, const char *text) {
    size_t i = 0;

    while (text[i] != '\0') {
        BufferStatus result = buffer_insert_char(eb, text[i]);
        assert(result == BUFFER_SUCCESS);
        i++;
    }
}

static void init_editor(Editor *editor, const char *text) {
    BufferStatus result = editor_init(editor, INITIAL_CAPACITY);
    assert(result == BUFFER_SUCCESS);

    fill_buffer(&editor->buffer, text);
    editor->preferred_column = buffer_get_cursor_column(&editor->buffer);
}

static void move_cursor_right(Editor *editor, size_t count) {
    size_t i;

    for (i = 0; i < count; i++) {
        BufferStatus result = buffer_move_right(&editor->buffer);
        assert(result == BUFFER_SUCCESS);
    }

    editor->preferred_column = buffer_get_cursor_column(&editor->buffer);
}

void test_editor_handle_key_gg(void) {
    printf("TESTING: editor_handle_key gg\n");

    Editor editor;
    init_editor(&editor, "abc\ndef");

    BufferStatus result = buffer_move_to_file_start(&editor.buffer);
    assert(result == BUFFER_SUCCESS);

    result = buffer_move_down(&editor.buffer, 2);
    assert(result == BUFFER_SUCCESS);
    editor.preferred_column = buffer_get_cursor_column(&editor.buffer);

    editor_handle_key(&editor, 'g');
    assert(editor.pending_motion_prefix == MOTION_PREFIX_G);
    assert(editor.buffer.gap_start == 6);

    editor_handle_key(&editor, 'g');
    assert(editor.pending_motion_prefix == MOTION_PREFIX_NONE);
    assert(editor.buffer.gap_start == 0);
    assert(editor.preferred_column == 0);

    editor_destroy(&editor);
    printf("PASSED: editor_handle_key gg\n");
}

void test_editor_handle_key_G(void) {
    printf("TESTING: editor_handle_key G\n");

    Editor editor;
    init_editor(&editor, "abc\ndef");

    BufferStatus result = buffer_move_to_file_start(&editor.buffer);
    assert(result == BUFFER_SUCCESS);
    editor.preferred_column = buffer_get_cursor_column(&editor.buffer);

    editor_handle_key(&editor, 'G');
    assert(editor.pending_motion_prefix == MOTION_PREFIX_NONE);
    assert(editor.buffer.gap_start == 7);
    assert(editor.preferred_column == 3);

    editor_destroy(&editor);
    printf("PASSED: editor_handle_key G\n");
}

void test_editor_handle_key_invalid_g_sequence(void) {
    printf("TESTING: editor_handle_key invalid g sequence\n");

    Editor editor;
    init_editor(&editor, "abc\ndef");

    BufferStatus result = buffer_move_to_file_start(&editor.buffer);
    assert(result == BUFFER_SUCCESS);

    result = buffer_move_down(&editor.buffer, 1);
    assert(result == BUFFER_SUCCESS);
    editor.preferred_column = buffer_get_cursor_column(&editor.buffer);

    editor_handle_key(&editor, 'g');
    assert(editor.pending_motion_prefix == MOTION_PREFIX_G);

    editor_handle_key(&editor, 'x');
    assert(editor.pending_motion_prefix == MOTION_PREFIX_NONE);
    assert(editor.buffer.gap_start == 5);
    assert(editor.preferred_column == 1);

    editor_handle_key(&editor, 'G');
    assert(editor.buffer.gap_start == 7);

    editor_destroy(&editor);
    printf("PASSED: editor_handle_key invalid g sequence\n");
}

void test_editor_handle_key_counted_vertical_motions(void) {
    printf("TESTING: editor_handle_key counted vertical motions\n");

    Editor editor;
    init_editor(&editor, "abcd\nxy\nwxyz\nmno");

    buffer_move_to_file_start(&editor.buffer);
    move_cursor_right(&editor, 3);

    editor_handle_key(&editor, '5');
    editor_handle_key(&editor, 'j');
    assert(editor.buffer.gap_start == 16);
    assert(editor.preferred_column == 3);
    assert(editor.pending_count == 0);

    editor_handle_key(&editor, '4');
    editor_handle_key(&editor, 'k');
    assert(editor.buffer.gap_start == 3);
    assert(editor.preferred_column == 3);
    assert(editor.pending_count == 0);

    editor_destroy(&editor);
    printf("PASSED: editor_handle_key counted vertical motions\n");
}

void test_editor_handle_key_counted_horizontal_motions(void) {
    printf("TESTING: editor_handle_key counted horizontal motions\n");

    Editor editor;
    init_editor(&editor, "abcd\nefgh");

    buffer_move_to_file_start(&editor.buffer);
    editor.preferred_column = buffer_get_cursor_column(&editor.buffer);

    editor_handle_key(&editor, '1');
    editor_handle_key(&editor, '2');
    editor_handle_key(&editor, 'l');
    assert(editor.buffer.gap_start == 4);
    assert(editor.preferred_column == 4);
    assert(editor.pending_count == 0);

    editor_handle_key(&editor, '1');
    editor_handle_key(&editor, '0');
    editor_handle_key(&editor, 'h');
    assert(editor.buffer.gap_start == 0);
    assert(editor.preferred_column == 0);
    assert(editor.pending_count == 0);

    editor_destroy(&editor);
    printf("PASSED: editor_handle_key counted horizontal motions\n");
}

void test_editor_handle_key_counted_word_motions(void) {
    printf("TESTING: editor_handle_key counted word motions\n");

    Editor editor;
    init_editor(&editor, "one two three four five");

    buffer_move_to_file_start(&editor.buffer);
    editor.preferred_column = buffer_get_cursor_column(&editor.buffer);

    editor_handle_key(&editor, '3');
    editor_handle_key(&editor, 'w');
    assert(editor.buffer.gap_start == 14);
    assert(editor.preferred_column == 14);
    assert(editor.pending_count == 0);

    editor_handle_key(&editor, '2');
    editor_handle_key(&editor, 'b');
    assert(editor.buffer.gap_start == 4);
    assert(editor.preferred_column == 4);
    assert(editor.pending_count == 0);

    buffer_move_to_file_start(&editor.buffer);
    editor.preferred_column = buffer_get_cursor_column(&editor.buffer);

    editor_handle_key(&editor, '4');
    editor_handle_key(&editor, 'e');
    assert(editor.buffer.gap_start == 17);
    assert(editor.preferred_column == 17);
    assert(editor.pending_count == 0);

    editor_destroy(&editor);
    printf("PASSED: editor_handle_key counted word motions\n");
}

void test_editor_handle_key_zero_count_and_line_start(void) {
    printf("TESTING: editor_handle_key zero count and line start\n");

    Editor editor;
    init_editor(&editor, "0\n1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n");

    buffer_move_to_file_start(&editor.buffer);
    editor.preferred_column = buffer_get_cursor_column(&editor.buffer);

    editor_handle_key(&editor, '1');
    editor_handle_key(&editor, '0');
    editor_handle_key(&editor, 'j');
    assert(editor.buffer.gap_start == 20);
    assert(editor.preferred_column == 0);
    assert(editor.pending_count == 0);

    editor_destroy(&editor);

    init_editor(&editor, "abcd\nef");
    buffer_move_to_file_start(&editor.buffer);
    buffer_move_down(&editor.buffer, 1);
    move_cursor_right(&editor, 1);

    editor_handle_key(&editor, '0');
    assert(editor.buffer.gap_start == 5);
    assert(editor.preferred_column == 0);
    assert(editor.pending_count == 0);

    editor_destroy(&editor);
    printf("PASSED: editor_handle_key zero count and line start\n");
}

void test_editor_handle_key_counted_line_motions(void) {
    printf("TESTING: editor_handle_key counted line motions\n");

    Editor editor;
    init_editor(&editor, "  aa\n b\n   ccc\n");

    buffer_move_to_file_start(&editor.buffer);
    editor.preferred_column = buffer_get_cursor_column(&editor.buffer);

    editor_handle_key(&editor, '2');
    editor_handle_key(&editor, '$');
    assert(editor.buffer.gap_start == 6);
    assert(editor.preferred_column == (size_t)-1);
    assert(editor.pending_count == 0);

    buffer_move_to_file_start(&editor.buffer);
    editor.preferred_column = buffer_get_cursor_column(&editor.buffer);

    editor_handle_key(&editor, '3');
    editor_handle_key(&editor, '^');
    assert(editor.buffer.gap_start == 11);
    assert(editor.preferred_column == 3);
    assert(editor.pending_count == 0);

    editor_destroy(&editor);
    printf("PASSED: editor_handle_key counted line motions\n");
}

void test_editor_handle_key_counted_absolute_line_motions(void) {
    printf("TESTING: editor_handle_key counted absolute line motions\n");

    Editor editor;
    init_editor(&editor, "aa\nbb\ncc\ndd");

    editor_handle_key(&editor, '3');
    editor_handle_key(&editor, 'g');
    assert(editor.pending_motion_prefix == MOTION_PREFIX_G);
    assert(editor.pending_count == 3);

    editor_handle_key(&editor, 'g');
    assert(editor.buffer.gap_start == 6);
    assert(editor.preferred_column == 0);
    assert(editor.pending_motion_prefix == MOTION_PREFIX_NONE);
    assert(editor.pending_count == 0);

    editor_handle_key(&editor, '2');
    editor_handle_key(&editor, 'G');
    assert(editor.buffer.gap_start == 3);
    assert(editor.preferred_column == 0);
    assert(editor.pending_count == 0);

    editor_handle_key(&editor, '9');
    editor_handle_key(&editor, 'G');
    assert(editor.buffer.gap_start == 9);
    assert(editor.preferred_column == 0);
    assert(editor.pending_count == 0);

    editor_destroy(&editor);
    printf("PASSED: editor_handle_key counted absolute line motions\n");
}

void test_editor_handle_key_invalid_counted_commands(void) {
    printf("TESTING: editor_handle_key invalid counted commands\n");

    Editor editor;
    init_editor(&editor, "abc\ndef");

    buffer_move_to_file_start(&editor.buffer);
    editor.preferred_column = buffer_get_cursor_column(&editor.buffer);

    editor_handle_key(&editor, '3');
    editor_handle_key(&editor, 'x');
    assert(editor.buffer.gap_start == 0);
    assert(editor.mode == MODE_NORMAL);
    assert(editor.pending_motion_prefix == MOTION_PREFIX_NONE);
    assert(editor.pending_count == 0);

    editor_handle_key(&editor, '4');
    editor_handle_key(&editor, 'g');
    assert(editor.pending_motion_prefix == MOTION_PREFIX_G);
    assert(editor.pending_count == 4);

    editor_handle_key(&editor, 'x');
    assert(editor.buffer.gap_start == 0);
    assert(editor.pending_motion_prefix == MOTION_PREFIX_NONE);
    assert(editor.pending_count == 0);

    editor_handle_key(&editor, '2');
    editor_handle_key(&editor, 'i');
    assert(editor.mode == MODE_NORMAL);
    assert(editor.buffer.gap_start == 0);
    assert(editor.pending_count == 0);

    editor_destroy(&editor);
    printf("PASSED: editor_handle_key invalid counted commands\n");
}

int main(void) {
    test_editor_handle_key_gg();
    test_editor_handle_key_G();
    test_editor_handle_key_invalid_g_sequence();
    test_editor_handle_key_counted_vertical_motions();
    test_editor_handle_key_counted_horizontal_motions();
    test_editor_handle_key_counted_word_motions();
    test_editor_handle_key_zero_count_and_line_start();
    test_editor_handle_key_counted_line_motions();
    test_editor_handle_key_counted_absolute_line_motions();
    test_editor_handle_key_invalid_counted_commands();

    printf("--- ALL EDITOR TESTS PASSED ---\n");

    return 0;
}
