// Copyright (C) 2019, Jian Lin
// Modified based on some built-in plugins of Claws-mail
// License: GPL v3

#include <glib.h>
#include <glib/gi18n.h>

#include <stdlib.h>

#include "version.h"
#include "claws.h"
#include "plugin.h"
#include "utils.h"
#include "hooks.h"
#include "compose.h"
#include "alertpanel.h"

#define PLUGIN_NAME (_("RuleWarn"))

gboolean rulewarn_hook(gpointer source, gpointer data)
{
    Compose *compose = (Compose *)source;
    gchar *clean_addr = NULL;
    gchar *cmd = NULL, *cmd_old = NULL;

    debug_print("rulewarn invoked\n");
    if (compose->batch) {
        return FALSE;    /* do not check while queuing */
    }

    gchar *rc_dir = get_rc_dir();
    cmd = g_strconcat("python ", rc_dir, "/RuleWarn.py", NULL);

    cmd_old = cmd;
    cmd = g_strconcat(cmd_old, " ", compose->account->address, NULL);
    g_free(cmd_old);

    GSList *cur;
    for (cur = compose->header_list; cur != NULL; cur = cur->next) {
        gchar *header;
        gchar *entry;
        header = gtk_editable_get_chars(GTK_EDITABLE(
                gtk_bin_get_child(GTK_BIN(
                    (((ComposeHeaderEntry *)cur->data)->combo)))), 0, -1);
        entry = gtk_editable_get_chars(GTK_EDITABLE(
                ((ComposeHeaderEntry *)cur->data)->entry), 0, -1);
        g_strstrip(entry);
        g_strstrip(header);

        clean_addr = g_strdup(entry);
	    extract_address(clean_addr);

        cmd_old = cmd;
        cmd = g_strconcat(cmd_old, " ", clean_addr, NULL);
        g_free(cmd_old);

        g_free(header);
        g_free(entry);
        g_free(clean_addr);
    }

    debug_print("RuleWarn script command: %s\n", cmd);

    if (cmd) { 
        AlertValue aval;
        gchar *message;

        int ret_code = -1;
        int status = system(cmd);
        g_free(cmd);

        if ((status >= 0) && (WIFEXITED(status))) {
            ret_code = WEXITSTATUS(status);
        }

        if (ret_code != 0) {
            message = g_strdup_printf(
                    _("Warning! Status: %d, Return code: %d.\n%s"),
                        status, ret_code, compose->sending?
                        _("Send it anyway?"):_("Queue it anyway?"));
            aval = alertpanel(_("RuleWarn"), message,
                    GTK_STOCK_CANCEL,
                        compose->sending ? _("_Send") : _("Queue"),
                        NULL,
                        ALERTFOCUS_SECOND);
            g_free(message);
            if (aval != G_ALERTALTERNATE) {
                return TRUE;
            }
        }
    }

    return FALSE;    /* continue sending */
}

static gulong hook_id = HOOK_NONE;

gint plugin_init(gchar **error)
{
    if (!check_plugin_version(MAKE_NUMERIC_VERSION(2,9,2,72),
                VERSION_NUMERIC, PLUGIN_NAME, error))
        return -1;

    hook_id = hooks_register_hook(COMPOSE_CHECK_BEFORE_SEND_HOOKLIST, rulewarn_hook, NULL);
    if (hook_id == HOOK_NONE) {
        *error = g_strdup(_("Failed to register hook"));
        return -1;
    }

    g_print("RuleWarn plugin loaded\n");

    return 0;
}

gboolean plugin_done(void)
{
    hooks_unregister_hook(COMPOSE_CHECK_BEFORE_SEND_HOOKLIST, hook_id);

    g_print("RuleWarn plugin unloaded\n");
    return TRUE;
}

const gchar *plugin_name(void)
{
    return PLUGIN_NAME;
}

const gchar *plugin_desc(void)
{
    return _("RuleWarn is a rule-based warning plugin. "
        "It uses the rule script ~/.claws-mail/RuleWarn.py "
        "to check the from/to addresses of mail to be sent, "
        "and shows a warning when the script exits "
        "with a non-zero status.");
}

const gchar *plugin_type(void)
{
    return "Common";
}

const gchar *plugin_licence(void)
{
    return "GPL3+";
}

const gchar *plugin_version(void)
{
    return VERSION;
}

struct PluginFeature *plugin_provides(void)
{
    static struct PluginFeature features[] = 
        { {PLUGIN_OTHER, N_("RuleWarn")},
          {PLUGIN_NOTHING, NULL}};
    return features;
}
