#include "editor.h"
#include "buffer.h"
#include <assert.h>
#include <stdio.h>

#define INITIAL_CAPACITY 16

static void fill_buffer(EditorBuffer *eb, const char *text) {
    size_t i = 0;

    while (text[i] != '\0') {
        BufferStatus result = buffer_insert(eb, text[i]);
        assert(result == BUFFER_SUCCESS);
        i++;
    }
}

static void init_editor(Editor *editor, const char *text) {
    BufferStatus result = buffer_init(&editor->buffer, INITIAL_CAPACITY);
    assert(result == BUFFER_SUCCESS);

    fill_buffer(&editor->buffer, text);

    editor->mode = MODE_NORMAL;
    editor->is_running = 1;
    editor->preferred_column = buffer_get_column(&editor->buffer);
    editor->pending_motion_prefix = MOTION_PREFIX_NONE;
}

void test_process_normal_mode_gg(void) {
    printf("TESTING: process_normal_mode gg\n");

    Editor editor;
    init_editor(&editor, "abc\ndef");

    BufferStatus result = buffer_move_file_start(&editor.buffer);
    assert(result == BUFFER_SUCCESS);

    result = buffer_move_down(&editor.buffer, 2);
    assert(result == BUFFER_SUCCESS);
    editor.preferred_column = buffer_get_column(&editor.buffer);

    process_normal_mode(&editor, 'g');
    assert(editor.pending_motion_prefix == MOTION_PREFIX_G);
    assert(editor.buffer.gap_start == 6);

    process_normal_mode(&editor, 'g');
    assert(editor.pending_motion_prefix == MOTION_PREFIX_NONE);
    assert(editor.buffer.gap_start == 0);
    assert(editor.preferred_column == 0);

    buffer_free(&editor.buffer);
    printf("PASSED: process_normal_mode gg\n");
}

void test_process_normal_mode_G(void) {
    printf("TESTING: process_normal_mode G\n");

    Editor editor;
    init_editor(&editor, "abc\ndef");

    BufferStatus result = buffer_move_file_start(&editor.buffer);
    assert(result == BUFFER_SUCCESS);
    editor.preferred_column = buffer_get_column(&editor.buffer);

    process_normal_mode(&editor, 'G');
    assert(editor.pending_motion_prefix == MOTION_PREFIX_NONE);
    assert(editor.buffer.gap_start == 7);
    assert(editor.preferred_column == 3);

    buffer_free(&editor.buffer);
    printf("PASSED: process_normal_mode G\n");
}

void test_process_normal_mode_invalid_g_sequence(void) {
    printf("TESTING: process_normal_mode invalid g sequence\n");

    Editor editor;
    init_editor(&editor, "abc\ndef");

    BufferStatus result = buffer_move_file_start(&editor.buffer);
    assert(result == BUFFER_SUCCESS);

    result = buffer_move_down(&editor.buffer, 1);
    assert(result == BUFFER_SUCCESS);
    editor.preferred_column = buffer_get_column(&editor.buffer);

    process_normal_mode(&editor, 'g');
    assert(editor.pending_motion_prefix == MOTION_PREFIX_G);

    process_normal_mode(&editor, 'x');
    assert(editor.pending_motion_prefix == MOTION_PREFIX_NONE);
    assert(editor.buffer.gap_start == 5);
    assert(editor.preferred_column == 1);

    process_normal_mode(&editor, 'G');
    assert(editor.buffer.gap_start == 7);

    buffer_free(&editor.buffer);
    printf("PASSED: process_normal_mode invalid g sequence\n");
}

int main(void) {
    test_process_normal_mode_gg();
    test_process_normal_mode_G();
    test_process_normal_mode_invalid_g_sequence();

    printf("--- ALL EDITOR TESTS PASSED ---\n");

    return 0;
}
