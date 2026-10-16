#include "event_log.h"
#include "contract.h"
#include <stddef.h>

/* Cấp phát bộ nhớ tĩnh, không dùng malloc (Chuẩn MISRA Dir 4.12) */
static event_node_t s_pool[EVENT_LOG_MAX_ENTRIES];
static uint32_t     s_pool_index = 0U;
static event_node_t *s_p_head = NULL;

void event_log_init(void) {
    s_pool_index = 0U;
    s_p_head = NULL;
}

status_t event_log_add(const event_node_t *p_event) {
    if (p_event == NULL) { return STATUS_ERR_NULL_PTR; }
    
    event_node_t *p_new_node = NULL;
    
    /* Lấy node từ Pool tĩnh, nếu đầy thì loại bỏ node cũ nhất (đuôi danh sách) để tái sử dụng */
    if (s_pool_index < EVENT_LOG_MAX_ENTRIES) {
        p_new_node = &s_pool[s_pool_index];
        s_pool_index++;
    } else {
        if (s_p_head == NULL) { return STATUS_ERR_EMPTY; }
        
        event_node_t *p_prev = NULL;
        event_node_t *p_curr = s_p_head;
        while (p_curr->p_next != NULL) {
            p_prev = p_curr;
            p_curr = p_curr->p_next;
        }
        
        if (p_prev != NULL) {
            p_prev->p_next = NULL;
        } else {
            s_p_head = NULL;
        }
        p_new_node = p_curr; 
    }
    
    p_new_node->type = p_event->type;
    p_new_node->timestamp = p_event->timestamp;
    p_new_node->value = p_event->value;
    p_new_node->p_next = NULL;
    
    /* Chèn theo thứ tự Timestamp (Mới nhất nằm ở đầu mảng Head) */
    if ((s_p_head == NULL) || (p_new_node->timestamp >= s_p_head->timestamp)) {
        p_new_node->p_next = s_p_head;
        s_p_head = p_new_node;
    } else {
        event_node_t *p_curr = s_p_head;
        while ((p_curr->p_next != NULL) && (p_curr->p_next->timestamp > p_new_node->timestamp)) {
            p_curr = p_curr->p_next;
        }
        p_new_node->p_next = p_curr->p_next;
        p_curr->p_next = p_new_node;
    }
    
    return STATUS_OK;
}

uint32_t event_log_get_count(void) {
    uint32_t count = 0U;
    event_node_t *p_curr = s_p_head;
    while (p_curr != NULL) {
        count++;
        p_curr = p_curr->p_next;
    }
    return count;
}

void event_log_print_all(void) {
    /* Hàm in ra file Log sẽ được tích hợp ở Task 7 */
}