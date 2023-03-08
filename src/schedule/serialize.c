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

static uint8_t* write_location(uint8_t *buf, location_t loc) {
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

    #undef WRITE_FLOAT

    return buf;
}

static uint8_t* read_location(uint8_t *buf, location_t *loc) {
    uint32_t le_32;
    #define READ_FLOAT(v) do {                                  \
            memcpy(&le_32, buf, sizeof(le_32));                 \
            le_32 = le32(le_32);                                \
            memcpy(&(v), &le_32, sizeof(le_32));                \
            buf += sizeof(le_32);                               \
        } while(0)

    READ_FLOAT(loc->altitude);
    READ_FLOAT(loc->bearing);
    READ_FLOAT(loc->distance);

    #undef READ_FLOAT

    return buf;
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

        switch(ev->tag) {
            case RWR_SCHEDULE_EVENT_NEWGUY: {
                buf = write_location(buf, ev->newguy.loc);
                le_16 = le16(ev->newguy.source);
                WRITE(le_16);
            } break;

            case RWR_SCHEDULE_EVENT_PAINT: {
                buf = write_location(buf, ev->paint.loc_diff);
            } break;

            case RWR_SCHEDULE_EVENT_FIRE_MISSILE: {
                le_16 = le16(ev->fire_missile.missile);
                WRITE(le_16);
            } break;

            case RWR_SCHEDULE_EVENT_DROP_MISSILE: {
                le_16 = le16(ev->drop_missile.missile);
                WRITE(le_16);
            } break;

            case RWR_SCHEDULE_EVENT_MISSILE_PING: {
                le_16 = le16(ev->missile_ping.missile);
                WRITE(le_16);
                buf = write_location(buf, ev->missile_ping.loc);
            } break;
        }

        buf = finish;
    }

    #undef WRITE
}

rwr_schedule_t* rwr_schedule_deserialize(void *vbuf, size_t len) {
    #define READ(v) do { memcpy(&(v), buf, sizeof(v)); buf += sizeof(v); } while(0)
    rwr_schedule_t *schedule = malloc(sizeof(*schedule));
    schedule->run.rwr = NULL;

    uint8_t *buf = (uint8_t*)vbuf;
    uint16_t le_16;
    READ(le_16);
    le_16 = le16(le_16);
    schedule->missiles = le_16;

    READ(le_16);
    le_16 = le16(le_16);
    schedule->contacts = le_16;

    uint32_t le_32;
    READ(le_32);
    le_32 = le32(le_32);
    schedule->events.len = le_32;
    schedule->events.cap = le_32;
    schedule->events.array = calloc(schedule->events.cap, sizeof(*schedule->events.array));

    for(uint32_t i = 0; i < schedule->events.len; ++i) {
        rwr_schedule_event_t *ev = &schedule->events.array[i];
        READ(le_16);
        le_16 = le16(le_16);
        ev->contact = le_16;

        READ(ev->tag);

        READ(le_32);
        le_32 = le32(le_32);
        ev->time_ms = le_32;

        uint8_t *finish = buf + RWR_SCHEDULE_EVENT_UNION_SIZE;

        switch(ev->tag) {
            case RWR_SCHEDULE_EVENT_NEWGUY: {
                buf = read_location(buf, &ev->newguy.loc);
                READ(le_16);
                le_16 = le16(le_16);
                ev->newguy.source = le_16;
            } break;

            case RWR_SCHEDULE_EVENT_PAINT: {
                buf = read_location(buf, &ev->paint.loc_diff);
            } break;

            case RWR_SCHEDULE_EVENT_FIRE_MISSILE: {
                READ(le_16);
                le_16 = le16(le_16);
                ev->fire_missile.missile = le_16;
            } break;

            case RWR_SCHEDULE_EVENT_DROP_MISSILE: {
                READ(le_16);
                le_16 = le16(le_16);
                ev->drop_missile.missile = le_16;
            } break;

            case RWR_SCHEDULE_EVENT_MISSILE_PING: {
                READ(le_16);
                le_16 = le16(le_16);
                ev->missile_ping.missile = le_16;
                buf = read_location(buf, &ev->missile_ping.loc);
            } break;
        }

        buf = finish;
    }

    #undef READ
    return schedule;

}
