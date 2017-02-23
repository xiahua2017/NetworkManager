/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/* NetworkManager system settings service
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * (C) Copyright 2008 Novell, Inc.
 * (C) Copyright 2008 - 2013 Red Hat, Inc.
 */

#ifndef __NETWORKMANAGER_SETTINGS_CONNECTION_H__
#define __NETWORKMANAGER_SETTINGS_CONNECTION_H__

#include <net/ethernet.h>

#include "nm-exported-object.h"
#include <nm-connection.h>

#define NM_TYPE_SETTINGS_CONNECTION            (nm_settings_connection_get_type ())
#define NM_SETTINGS_CONNECTION(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), NM_TYPE_SETTINGS_CONNECTION, NMSettingsConnection))
#define NM_SETTINGS_CONNECTION_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), NM_TYPE_SETTINGS_CONNECTION, NMSettingsConnectionClass))
#define NM_IS_SETTINGS_CONNECTION(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), NM_TYPE_SETTINGS_CONNECTION))
#define NM_IS_SETTINGS_CONNECTION_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), NM_TYPE_SETTINGS_CONNECTION))
#define NM_SETTINGS_CONNECTION_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), NM_TYPE_SETTINGS_CONNECTION, NMSettingsConnectionClass))

/* Signals */
#define NM_SETTINGS_CONNECTION_UPDATED "updated"
#define NM_SETTINGS_CONNECTION_REMOVED "removed"
#define NM_SETTINGS_CONNECTION_GET_SECRETS "get-secrets"
#define NM_SETTINGS_CONNECTION_CANCEL_SECRETS "cancel-secrets"

/* Internal signals */
#define NM_SETTINGS_CONNECTION_UPDATED_INTERNAL "updated-internal"

/* Properties */
#define NM_SETTINGS_CONNECTION_VISIBLE  "visible"
#define NM_SETTINGS_CONNECTION_UNSAVED  "unsaved"
#define NM_SETTINGS_CONNECTION_READY    "ready"
#define NM_SETTINGS_CONNECTION_FLAGS    "flags"
#define NM_SETTINGS_CONNECTION_FILENAME "filename"


/**
 * NMSettingsConnectionFlags:
 * @NM_SETTINGS_CONNECTION_FLAGS_NONE: no flag set
 * @NM_SETTINGS_CONNECTION_FLAGS_UNSAVED: the connection is not saved to disk
 * @NM_SETTINGS_CONNECTION_FLAGS_NM_GENERATED: A connection is "nm-generated" if
 *  it was generated by NetworkManger. If the connection gets modified or saved
 *  by the user, the flag gets cleared. A nm-generated is implicitly unsaved.
 * @NM_SETTINGS_CONNECTION_FLAGS_NM_GENERATED_ASSUMED: A special kind of "nm-generated"
 *  connection that was specifically created for connection assumption. "nm-generated-assumed"
 *  implies "nm-generated".
 * @NM_SETTINGS_CONNECTION_FLAGS_ALL: special mask, for all known flags
 *
 * #NMSettingsConnection flags.
 **/
typedef enum
{
	NM_SETTINGS_CONNECTION_FLAGS_NONE                               = 0x00,
	NM_SETTINGS_CONNECTION_FLAGS_UNSAVED                            = 0x01,
	NM_SETTINGS_CONNECTION_FLAGS_NM_GENERATED                       = 0x02,
	NM_SETTINGS_CONNECTION_FLAGS_NM_GENERATED_ASSUMED               = 0x04,

	__NM_SETTINGS_CONNECTION_FLAGS_LAST,
	NM_SETTINGS_CONNECTION_FLAGS_ALL = ((__NM_SETTINGS_CONNECTION_FLAGS_LAST - 1) << 1) - 1,
} NMSettingsConnectionFlags;

typedef enum { /*< skip >*/
	NM_SETTINGS_CONNECTION_COMMIT_REASON_NONE                       = 0,
	NM_SETTINGS_CONNECTION_COMMIT_REASON_USER_ACTION                = (1LL << 0),
	NM_SETTINGS_CONNECTION_COMMIT_REASON_ID_CHANGED                 = (1LL << 1),
} NMSettingsConnectionCommitReason;

typedef enum {
	NM_SETTINGS_AUTO_CONNECT_BLOCKED_REASON_UNBLOCKED               = 0,
	NM_SETTINGS_AUTO_CONNECT_BLOCKED_REASON_BLOCKED                 = 1,
	NM_SETTINGS_AUTO_CONNECT_BLOCKED_REASON_NO_SECRETS              = 2,
} NMSettingsAutoconnectBlockedReason;

struct _NMSettingsConnectionCallId;
typedef struct _NMSettingsConnectionCallId *NMSettingsConnectionCallId;

typedef struct _NMSettingsConnectionClass NMSettingsConnectionClass;

typedef void (*NMSettingsConnectionCommitFunc) (NMSettingsConnection *self,
                                                GError *error,
                                                gpointer user_data);

typedef void (*NMSettingsConnectionDeleteFunc) (NMSettingsConnection *self,
                                                GError *error,
                                                gpointer user_data);

struct _NMSettingsConnectionPrivate;

struct _NMSettingsConnection {
	NMExportedObject parent;
	struct _NMSettingsConnectionPrivate *_priv;
};

struct _NMSettingsConnectionClass {
	NMExportedObjectClass parent;

	/* virtual methods */
	void (*replace_and_commit) (NMSettingsConnection *self,
	                            NMConnection *new_connection,
	                            NMSettingsConnectionCommitFunc callback,
	                            gpointer user_data);

	void (*commit_changes) (NMSettingsConnection *self,
	                        NMSettingsConnectionCommitReason commit_reason,
	                        NMSettingsConnectionCommitFunc callback,
	                        gpointer user_data);

	void (*delete) (NMSettingsConnection *self,
	                NMSettingsConnectionDeleteFunc callback,
	                gpointer user_data);

	gboolean (*supports_secrets) (NMSettingsConnection *self,
	                              const char *setting_name);
};

GType nm_settings_connection_get_type (void);

gboolean nm_settings_connection_has_unmodified_applied_connection (NMSettingsConnection *self,
                                                                   NMConnection *applied_connection,
                                                                   NMSettingCompareFlags compare_flage);

void nm_settings_connection_commit_changes (NMSettingsConnection *self,
                                            NMSettingsConnectionCommitReason commit_reason,
                                            NMSettingsConnectionCommitFunc callback,
                                            gpointer user_data);

gboolean nm_settings_connection_replace_settings (NMSettingsConnection *self,
                                                  NMConnection *new_connection,
                                                  gboolean update_unsaved,
                                                  const char *log_diff_name,
                                                  GError **error);

void nm_settings_connection_replace_and_commit (NMSettingsConnection *self,
                                                NMConnection *new_connection,
                                                NMSettingsConnectionCommitFunc callback,
                                                gpointer user_data);

void nm_settings_connection_delete (NMSettingsConnection *self,
                                    NMSettingsConnectionDeleteFunc callback,
                                    gpointer user_data);

typedef void (*NMSettingsConnectionSecretsFunc) (NMSettingsConnection *self,
                                                 NMSettingsConnectionCallId call_id,
                                                 const char *agent_username,
                                                 const char *setting_name,
                                                 GError *error,
                                                 gpointer user_data);

NMSettingsConnectionCallId nm_settings_connection_get_secrets (NMSettingsConnection *self,
                                                               NMConnection *applied_connection,
                                                               NMAuthSubject *subject,
                                                               const char *setting_name,
                                                               NMSecretAgentGetSecretsFlags flags,
                                                               const char **hints,
                                                               NMSettingsConnectionSecretsFunc callback,
                                                               gpointer callback_data);

void nm_settings_connection_cancel_secrets (NMSettingsConnection *self,
                                            NMSettingsConnectionCallId call_id);

gboolean nm_settings_connection_is_visible (NMSettingsConnection *self);

void nm_settings_connection_recheck_visibility (NMSettingsConnection *self);

gboolean nm_settings_connection_check_permission (NMSettingsConnection *self,
                                                  const char *permission);

void nm_settings_connection_signal_remove (NMSettingsConnection *self, gboolean allow_reuse);

gboolean nm_settings_connection_get_unsaved (NMSettingsConnection *self);

NMSettingsConnectionFlags nm_settings_connection_get_flags (NMSettingsConnection *self);
NMSettingsConnectionFlags nm_settings_connection_set_flags (NMSettingsConnection *self, NMSettingsConnectionFlags flags, gboolean set);
NMSettingsConnectionFlags nm_settings_connection_set_flags_all (NMSettingsConnection *self, NMSettingsConnectionFlags flags);

int nm_settings_connection_cmp_timestamp (NMSettingsConnection *ac, NMSettingsConnection *ab);
int nm_settings_connection_cmp_timestamp_p_with_data (gconstpointer pa, gconstpointer pb, gpointer user_data);
int nm_settings_connection_cmp_autoconnect_priority (NMSettingsConnection *a, NMSettingsConnection *b);
int nm_settings_connection_cmp_autoconnect_priority_p_with_data (gconstpointer pa, gconstpointer pb, gpointer user_data);

gboolean nm_settings_connection_get_timestamp (NMSettingsConnection *self,
                                               guint64 *out_timestamp);

void nm_settings_connection_update_timestamp (NMSettingsConnection *self,
                                              guint64 timestamp,
                                              gboolean flush_to_disk);

void nm_settings_connection_read_and_fill_timestamp (NMSettingsConnection *self);

char **nm_settings_connection_get_seen_bssids (NMSettingsConnection *self);

gboolean nm_settings_connection_has_seen_bssid (NMSettingsConnection *self,
                                                const char *bssid);

void nm_settings_connection_add_seen_bssid (NMSettingsConnection *self,
                                            const char *seen_bssid);

void nm_settings_connection_read_and_fill_seen_bssids (NMSettingsConnection *self);

int nm_settings_connection_get_autoconnect_retries (NMSettingsConnection *self);
void nm_settings_connection_set_autoconnect_retries (NMSettingsConnection *self,
                                                     int retries);
void nm_settings_connection_reset_autoconnect_retries (NMSettingsConnection *self);

gint32 nm_settings_connection_get_autoconnect_retry_time (NMSettingsConnection *self);

NMSettingsAutoconnectBlockedReason nm_settings_connection_get_autoconnect_blocked_reason (NMSettingsConnection *self);
void                               nm_settings_connection_set_autoconnect_blocked_reason (NMSettingsConnection *self,
                                                                                          NMSettingsAutoconnectBlockedReason reason);

gboolean nm_settings_connection_can_autoconnect (NMSettingsConnection *self);

gboolean nm_settings_connection_get_nm_generated (NMSettingsConnection *self);
gboolean nm_settings_connection_get_nm_generated_assumed (NMSettingsConnection *self);

gboolean nm_settings_connection_get_ready (NMSettingsConnection *self);
void     nm_settings_connection_set_ready (NMSettingsConnection *self,
                                           gboolean ready);

void        nm_settings_connection_set_filename (NMSettingsConnection *self,
                                                 const char *filename);
const char *nm_settings_connection_get_filename (NMSettingsConnection *self);

const char *nm_settings_connection_get_id   (NMSettingsConnection *connection);
const char *nm_settings_connection_get_uuid (NMSettingsConnection *connection);

#endif /* __NETWORKMANAGER_SETTINGS_CONNECTION_H__ */
