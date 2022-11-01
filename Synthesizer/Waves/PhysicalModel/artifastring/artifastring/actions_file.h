/*
 * Copyright 2010--2013 Graham Percival
 * This file is part of Artifastring.
 *
 * Artifastring is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * Artifastring is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with Artifastring.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifndef ACTIONS_FILE
#define ACTIONS_FILE
#include <stdio.h>

// we now pass multi-line comments for NoteBeginning and NoteEnding

class ActionsFile {
public:
    /**
     * @brief mundane constructor.
     * @param[in] filename Filename; \c ".actions" is recommended.
     * @param[in] buffer_size Number of actions to store
     * before writing to disk.
     * @warning ActionsFile does not check whether it received the
     * memory it attempted to allocate; if this occurs, it will
     * probably result in an unchecked exception crash.
     */
    ActionsFile(const char *filename, int buffer_size=1024);

    /// @brief writes data to disk before quitting
    ~ActionsFile();

    void close();

    void wait(float seconds);
    void skipStart(float seconds);
    void skipStop(float seconds);
    void finger(float seconds, int string_number,
                float position);
    void pluck(float seconds, int string_number,
               float position, float force);
    void bow(float seconds, int string_number,
             float position, float force, float velocity,
             float bow_pos_along);
    void accel(float seconds, int string_number,
               float position, float force, float velocity,
               float bow_pos_along, float bow_accel);

    void category(float seconds, float category);
    // writes buffer to file immediately
    void comment(const char *text);

private:
    void writeBuffer();

    int size;
    int index;
    FILE *outfile;

    enum ActionType {
        ACTION_SKIPSTART,
        ACTION_SKIPSTOP,
        ACTION_WAIT,
        ACTION_FINGER,
        ACTION_PLUCK,
        ACTION_BOW,
        ACTION_ACCEL,
        ACTION_CATEGORY
    };

    typedef struct {
        ActionType type;
        float seconds;
        int string_number;
        float position;
        float force;
        float velocity;
        float position_along;
        float accel;
    } ActionData;

    ActionData *data;
};
#endif

