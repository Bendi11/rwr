#pragma once
#include "rwr/model/alr56.h"

enum {
    ALR56_CB_BLINK,
    ALR56_CB_CHECKPING,
};

#define ALR56_LAUNCH_PERIOD (0.1f)
#define ALR56_LAUNCH_REPETITIONS (10)
#define ALR56_MS_BEFORE_DROP (5000)
#define ALR56_UPDATE_INTERVAL_MS (1000)
#define ALR56_MISSILE_RECYCLE_INTERVAL_MS (15000)


/// Check if the given contact has been forgotten by the RWR because it has not been pinged
bool alr56_contact_forgotten_impl(alr56_t *rwr, const contact_t *contact);

/// Paint the RWR receiver with radar, updating the position on the display
contact_t* alr56_ping_impl(alr56_t *rwr, contact_t *contact, location_t loc);

/// Drop the given contact from the RWR, freeing the memory allocated for the contact
void alr56_drop_impl(alr56_t *rwr, contact_t *contact);

/// Upgrade the given contact to an STT lock
void alr56_lock_impl(alr56_t *rwr, contact_t *contact);

/// Launch a missile from the given target
fired_missile_t* alr56_missile_impl(alr56_t *rwr, contact_t *contact);

/// Drop the given RWR contact's lock if gained
void alr56_drop_lock_impl(alr56_t *rwr, contact_t *contact);

/// SDL timer callback to periodically update the RWR display, dropping contacts if no pings were received
unsigned int alr56_periodic_cb(unsigned int _, void *rwr);

/// Create a new SDL timer that will toggle the given blink state until it turns off
void alr56_blink_timer_set(alr56_blink_common_t *blink);

/// Move the given contact to the forgotten list
contact_t* alr56_forget_contact_impl(alr56_t *rwr, contact_t *contact);

/// Move the given contact from the forgotten list to the active contacts list, if there is an empty spot on the contacts list
contact_t* alr56_remember_contact(alr56_t *rwr, contact_t *contact);

/// Remove a contact from the forgotten contacts list
void alr56_remove_forgotten(alr56_t *rwr, contact_t *contact);

/// Clear the current priority target
void alr56_clear_priority(alr56_t *rwr);

/// Find the contact with the highest value returned from `alr56_get_threat`
contact_t* alr56_find_priority(alr56_t *rwr);

/// Convert a PRF description into a lock tone that can be played
tone_sequence_t* alr56_get_lock_tone(alr56_t *rwr, const source_t *const source);

/// Get a number representing the threat posed by a radar contact, weighing distance and current lock status
uint16_t alr56_get_threat(alr56_t *rwr, const contact_t *source);

/// Remove the currently prioritized contact and recalculate the priority contact for floating diamond handoff mode
///
/// Does not remove the currently playing lock tone if the priority contact does not change
void alr56_recompute_priority(alr56_t *rwr);

tone_sequence_t* alr56_newguy_air_tone(void);

tone_sequence_t* alr56_newguy_surface_tone(void);

tone_sequence_t* alr56_missile_tone(void);

tone_sequence_t* alr56_missile_recycle_tone(void);

tone_sequence_t* alr56_silence_tone(void);
