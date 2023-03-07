#include "private.h"
#include "rwr/schedule/schedule.h"

static uint16_t endian = 0x00FF;
#define IS_LITTLE_ENDIAN ((*(uint8_t*)&endian) == 0xFF)
#define IS_BIG_ENDIAN ((*(uint8_t*)&endian) == 0x00)

static uint16_t le16(uint16_t read) {
    if(IS_LITTLE_ENDIAN) { return read; }
    else { return (uint8_t)(read >> 8) | (uint8_t)(read << 8); }
}

static uint32_t le32(uint32_t read) {
    if(IS_LITTLE_ENDIAN) {
        return read;
    } else {
        return (uint8_t)(read >> 24) |
            (uint8_t)((read << 8) & 0xff0000) |
            (uint8_t)((read >> 8) & 0xff00) |
            (uint8_t)((read << 24) & 0xff000000);
    }
}

size_t rwr_schedule_serialized_len(rwr_schedule_t *schedule) {
    return sizeof(schedule->missiles) +
        sizeof(schedule->contacts) +
        sizeof(*schedule->events.array) * schedule->events.len +
        sizeof(schedule->events.len);
}

static void write_location(uint8_t *buf, location_t loc) {
    uint32_t le_32;
    #define WRITE_FLOAT(v) do {                                  \
            memcpy(&le_32, &(v), sizeof(v)); \
            le_32 = le32(le_32);                                 \
            memcpy(buf, &le_32, sizeof(le_32));                  \
            buf += sizeof(le_32);                                \
        } while(0)
    
    WRITE_FLOAT(loc.altitude);
    WRITE_FLOAT(loc.bearing);
    WRITE_FLOAT(loc.distance);
}

void rwr_schedule_serialize(rwr_schedule_t *schedule, void *vbuf) {
    #define WRITE(v) do { memcpy(buf, &(v), sizeof(v)); buf += sizeof(v); } while(0)
    uint8_t *buf = (uint8_t*)vbuf;
    uint16_t le_16 = le16(schedule->missiles);
    WRITE(le_16);
    le_16 = le16(schedule->contacts);
    WRITE(le_16);
    uint32_t le_32 = le32(schedule->events.len);
    WRITE(le_32);

    for(uint32_t i = 0; i < schedule->events.len; ++i) {
        rwr_schedule_event_t *ev = &schedule->events.array[i];
        le_16 = le16(ev->contact);
        WRITE(le_16);
        WRITE(ev->tag);
        le_32 = le32(ev->time_ms);
        WRITE(le_32);

        uint8_t *finish = buf + RWR_SCHEDULE_EVENT_UNION_SIZE;

        switch(schedule->events.array[i].tag) {
            case RWR_SCHEDULE_EVENT_NEWGUY: {
                write_location(buf, ev->newguy.loc);
                le_16 = le16(ev->newguy.source);
                WRITE(le_16);
            } break;
        }

        buf = finish;
    }
}
