/*
 * Split half connection indicator for the central (left) half.
 * SPDX-License-Identifier: MIT
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/display.h>
#include <zmk/display/widgets/split_half_status.h>
#include <zmk/event_manager.h>
#include <zmk/events/position_state_changed.h>
#include <zmk/events/battery_state_changed.h>
#include <zmk/split/transport/types.h>
#include <zmk/split/transport/central.h>

extern const struct zmk_split_transport_central *active_transport;

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);

struct split_half_status_state {
    bool connected;
};

static struct split_half_status_state split_half_status_get_state(const zmk_event_t *eh) {
    ARG_UNUSED(eh);

    bool connected = false;

    if (active_transport != NULL && active_transport->api != NULL &&
        active_transport->api->get_status != NULL) {
        enum zmk_split_transport_connections_status status =
            active_transport->api->get_status().connections;

        connected = (status == ZMK_SPLIT_TRANSPORT_CONNECTIONS_STATUS_ALL_CONNECTED);
    }

    return (struct split_half_status_state){.connected = connected};
}

static void set_split_half_symbol(lv_obj_t *label, struct split_half_status_state state) {
    const char *text = state.connected ? "R " LV_SYMBOL_OK : "R " LV_SYMBOL_CLOSE;

    lv_label_set_text(label, text);
}

static void split_half_status_update_cb(struct split_half_status_state state) {
    struct zmk_widget_split_half_status *widget;

    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) {
        set_split_half_symbol(widget->obj, state);
    }
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_split_half_status, struct split_half_status_state,
                            split_half_status_update_cb, split_half_status_get_state)

ZMK_SUBSCRIPTION(widget_split_half_status, zmk_position_state_changed);
ZMK_SUBSCRIPTION(widget_split_half_status, zmk_peripheral_battery_state_changed);

static void split_half_status_timer_handler(struct k_timer *timer) {
    ARG_UNUSED(timer);
    split_half_status_update_cb(split_half_status_get_state(NULL));
}

K_TIMER_DEFINE(split_half_status_timer, split_half_status_timer_handler, NULL);

int zmk_widget_split_half_status_init(struct zmk_widget_split_half_status *widget,
                                      lv_obj_t *parent) {
    widget->obj = lv_label_create(parent);

    sys_slist_append(&widgets, &widget->node);

    widget_split_half_status_init();

    k_timer_start(&split_half_status_timer, K_SECONDS(1), K_SECONDS(2));

    return 0;
}

lv_obj_t *zmk_widget_split_half_status_obj(struct zmk_widget_split_half_status *widget) {
    return widget->obj;
}
