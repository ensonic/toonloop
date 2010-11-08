/*
 * Toonloop
 *
 * Copyright 2010 Alexandre Quessy
 * <alexandre@quessy.net>
 * http://www.toonloop.com
 *
 * Toonloop is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Toonloop is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the gnu general public license
 * along with Toonloop.  If not, see <http://www.gnu.org/licenses/>.
 */

// TODO: rename to midibinder.cpp

#include "presets.h"
#include "unused.h"
#include <boost/lexical_cast.hpp>
#include <glib.h>
#include <iostream>
#include <map>
#include <stdexcept>
#include <stdio.h>
#include <string>
#include <vector>
// TODO:2010-11-07:aalex:override << for MidiRule
#if 0
std::ostream &MidiRule::operator<< (std::ostream &theStream, const MidiRule &self)
{
    theStream << "MidiRule: type=" << self.type_ << " action=" << self.action_ << " args=" << self.args_ << " number=" << self.number_ << " from|to=" << self.from_ << " " << self.to_;
    return theStream;
}
#endif
/**
 * Returns 0 if none found.
 */
const MidiRule *MidiBinder::find_program_change_rule()
{
    std::cout << __FUNCTION__ << std::endl;
    if (program_change_rules_.size() >= 1)
        try {
            //std::cout << program_change_rules_.at(0) << std::endl;
            std::cout << "Found rule for program change!" << std::endl;
            return &(program_change_rules_.at(0));
        } catch (std::out_of_range &e) {
            std::cout << "Program change rule: out of range! " << e.what() << std::endl;
            return 0;
        }
    else
        return 0;
}
/**
 * Returns 0 if none found.
 */
const MidiRule *MidiBinder::find_rule(RuleType rule_type, int number)
{
    std::cout << "Method: MidiBinder::" << __FUNCTION__  << " Looking for " << number << std::endl;
    std::vector<MidiRule> rules;
    switch (rule_type)
    {
        case NOTE_ON_RULE:
            rules = note_on_rules_;
            break;
        case NOTE_OFF_RULE:
            rules = note_off_rules_;
            break;
        case CONTROL_ON_RULE:
            rules = control_on_rules_;
            break;
        case CONTROL_OFF_RULE:
            rules = control_off_rules_;
            break;
        case CONTROL_MAP_RULE:
            rules = control_map_rules_;
            break;
        default:
            g_critical("Unsupported rule type");
            return 0;
            break;
    }
    for (MidiRuleIterator iter = rules.begin(); iter != rules.end(); ++iter)
    {
        std::cout << " - Rule: action=" << (*iter).action_ << " number=" << (*iter).number_ << std::endl;
        if ((*iter).number_ == number)
        {
            std::cout << "Found rule !" << std::endl;
            //std::cout << *iter << std::endl;
            return &(*iter);
        }
    }
    return 0;
}

/*
 * Called for open tags <foo bar="baz">
 */
void MidiBinder::on_midi_xml_start_element(
        GMarkupParseContext *context,
        const gchar *element_name,
        const gchar **attribute_names,
        const gchar **attribute_values,
        gpointer user_data,
        GError **error)
{
    UNUSED(context);
    UNUSED(error);
    MidiBinder *self = static_cast<MidiBinder *>(user_data);

    MidiRule rule;
    // default values:
    rule.action_ = "";
    rule.args_ = "";
    rule.from_ = 0.0f;
    rule.to_ = 0.0f;
    rule.number_ = 0;
    if (g_strcmp0(element_name, "toonloop_midi_rules") == 0) // root
        return;
    else if (g_strcmp0(element_name, "note_on") == 0)
        rule.type_ = NOTE_ON_RULE;
    else if (g_strcmp0(element_name, "note_off") == 0)
        rule.type_ = NOTE_OFF_RULE;
    else if (g_strcmp0(element_name, "control_on") == 0)
        rule.type_ = CONTROL_ON_RULE;
    else if (g_strcmp0(element_name, "control_off") == 0)
        rule.type_ = CONTROL_OFF_RULE;
    else if (g_strcmp0(element_name, "control_map") == 0)
        rule.type_ = CONTROL_MAP_RULE;
    else if (g_strcmp0(element_name, "program_change") == 0)
        rule.type_ = PROGRAM_CHANGE_RULE;
    else
        g_critical("Invalid MIDI rule type: %s", element_name);
    std::cout << "Adding rule " << element_name << ":";
    const gchar **name_cursor = attribute_names;
    const gchar **value_cursor = attribute_values;
    while (*name_cursor)
    {
        std::cout << " " << *name_cursor << "=" << *value_cursor;
        if (g_strcmp0(*name_cursor, "args") == 0)
            rule.args_ = *value_cursor;
        if (g_strcmp0(*name_cursor, "action") == 0)
            rule.action_ = *value_cursor;
        else if (g_strcmp0(*name_cursor, "number") == 0)
        {
            try {
                rule.number_ = boost::lexical_cast<int>(*value_cursor);
            } catch(boost::bad_lexical_cast &) {
                g_critical("Invalid int for %s in XML file: %s", *name_cursor, *value_cursor);
                return;
            }
        }
        else if (g_strcmp0(*name_cursor, "from") == 0)
        {
            try {
                rule.from_ = boost::lexical_cast<float>(*value_cursor);
            } catch(boost::bad_lexical_cast &) {
                g_critical("Invalid int for %s in XML file: %s", *name_cursor, *value_cursor);
                return;
            }
        }
        else if (g_strcmp0(*name_cursor, "to") == 0)
        {
            try {
                rule.to_ = boost::lexical_cast<float>(*value_cursor);
            } catch(boost::bad_lexical_cast &) {
                g_critical("Invalid int for %s in XML file: %s", *name_cursor, *value_cursor);
                return;
            }
        }
        name_cursor++;
        value_cursor++;
    }
    std::cout << std::endl;
    if (rule.action_ == "")
    {
        g_critical("No action for rule %s", element_name);
        return;
    }
    if (rule.type_ == NOTE_ON_RULE)
        self->note_on_rules_.push_back(rule);
    else if (rule.type_ == NOTE_OFF_RULE)
        self->note_off_rules_.push_back(rule);
    else if (rule.type_ == CONTROL_ON_RULE)
        self->control_on_rules_.push_back(rule);
    else if (rule.type_ == CONTROL_OFF_RULE)
        self->control_off_rules_.push_back(rule);
    else if (rule.type_ == CONTROL_MAP_RULE)
        self->control_map_rules_.push_back(rule);
    else if (rule.type_ == PROGRAM_CHANGE_RULE)
        self->program_change_rules_.push_back(rule);
}

/** 
 * Called on error, including one set by other
 * methods in the vtable. The GError should not be freed.
 */
static void on_midi_xml_error(GMarkupParseContext *context, GError *error, gpointer user_data)
{
    UNUSED(context);
    UNUSED(user_data);
    //MidiBinder *self = static_cast<MidiBinder *>(user_data);
    g_print("Error parsing XML markup: %s\n", error->message);
}

/**
 * Finds a preset file somewhere in the location it could be.
 * 
 * These are the presets dirs in /usr/share/toonloop/presets and the like.
 */
gchar *toon_find_midi_preset_file(const gchar *file_name)
{
    // TODO: add ~/.toonloop/ first
    const gchar *dirs[] ={"", "../presets/", "./presets/", PRESETS_DIR, NULL};
    int i;
    for (i = 0; dirs[i]; i++)
    {
        gchar *path = g_strdup_printf("%s%s", dirs[i], file_name);
        if (g_file_test(path, G_FILE_TEST_EXISTS))
            return path;
        g_free(path);
    }
    return NULL;
}
/**
 * Code to load the XML file into memory and parse it. 
 */
bool MidiBinder::load_xml_file(const gchar *file_name)
{
    /* The list of what handler does what. */
    GMarkupParser parser = {
        MidiBinder::on_midi_xml_start_element,
        NULL,
        NULL,
        NULL,
        on_midi_xml_error
    };
    gpointer user_data = (gpointer) this;
    GMarkupParseContext *context = g_markup_parse_context_new(&parser, G_MARKUP_TREAT_CDATA_AS_TEXT, user_data, NULL);
    /* seriously crummy error checking */
    char *xml_text;
    gsize length;
    if (g_file_get_contents(file_name, &xml_text, &length, NULL) == FALSE)
    {
        printf("Couldn't load XML\n");
        return false;
    }
    if (g_markup_parse_context_parse(context, xml_text, length, NULL) == FALSE)
    {
        printf("Parse failed\n");
        return false;
    }
    g_free(xml_text);
    g_markup_parse_context_free(context);
    return true;
}

MidiBinder::MidiBinder()
{
    std::string full_name(toon_find_midi_preset_file("midi.xml"));
    std::cout << "Found MIDI bindings file " << full_name << std::endl;
    
    if (load_xml_file(full_name.c_str()))
        g_print("successfully loaded XML file\n");
}

