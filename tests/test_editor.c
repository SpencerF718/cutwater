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

int main(void) {
    test_editor_handle_key_gg();
    test_editor_handle_key_G();
    test_editor_handle_key_invalid_g_sequence();

    printf("--- ALL EDITOR TESTS PASSED ---\n");

    return 0;
}
