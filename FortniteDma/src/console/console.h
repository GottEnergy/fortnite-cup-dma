#pragma once

#include <algorithm>
#include <iostream>
#include <list>
#include <map>
#include <string>
#include <type_traits>
#include <utility>
#include <windows.h>

namespace hue
{
    inline constexpr int DEFAULT_COLOR = 7;
    inline constexpr int BAD_COLOR = -256;

    inline const std::map<std::string, int> CODES = {
        {"black",            0}, {"k",   0},
        {"blue",             1}, {"b",   1},
        {"green",            2}, {"g",   2},
        {"aqua",             3}, {"a",   3},
        {"red",              4}, {"r",   4},
        {"purple",           5}, {"p",   5},
        {"yellow",           6}, {"y",   6},
        {"white",            7}, {"w",   7},
        {"grey",             8}, {"e",   8},
        {"light blue",       9}, {"lb",  9},
        {"light green",     10}, {"lg", 10},
        {"light aqua",      11}, {"la", 11},
        {"light red",       12}, {"lr", 12},
        {"light purple",    13}, {"lp", 13},
        {"light yellow",    14}, {"ly", 14},
        {"bright white",    15}, {"bw", 15}
    };

    inline const std::map<int, std::string> NAMES = {
        { 0, "black"},
        { 1, "blue"},
        { 2, "green"},
        { 3, "aqua"},
        { 4, "red"},
        { 5, "purple"},
        { 6, "yellow"},
        { 7, "white"},
        { 8, "grey"},
        { 9, "light blue"},
        {10, "light green"},
        {11, "light aqua"},
        {12, "light red"},
        {13, "light purple"},
        {14, "light yellow"},
        {15, "bright white"}
    };

    inline bool is_good(int c)
    {
        return 0 <= c && c < 256;
    }

    inline int itoc(int c)
    {
        return is_good(c) ? c : BAD_COLOR;
    }

    inline int itoc(int a, int b)
    {
        return itoc(a + b * 16);
    }

    // std::string to color
    inline int stoc(std::string a)
    {
        // convert s to lowercase, and format variants like  "light_blue"
        std::transform(a.begin(), a.end(), a.begin(), [](char c)
            {
                if ('A' <= c && c <= 'Z')
                    c = c - 'A' + 'a';
                else if (c == '_' || c == '-')
                    c = ' ';
                return c;
            });

        // operator[] on std::map is non-const, use std::map::at instead
        return (CODES.find(a) != CODES.end()) ? CODES.at(a) : BAD_COLOR;
    }

    inline int stoc(std::string a, std::string b)
    {
        return itoc(stoc(a), stoc(b));
    }

    inline std::string ctos(int c)
    {
        return (0 <= c && c < 256) ?
            "(text) " + NAMES.at(c % 16) + TEXT(" + ") +
            "(background) " + NAMES.at(c / 16) :
            "BAD COLOR";
    }

    inline int get()
    {
        CONSOLE_SCREEN_BUFFER_INFO i;
        return GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &i) ?
            i.wAttributes : BAD_COLOR;
    }

    inline int get_text()
    {
        return (get() != BAD_COLOR) ? get() % 16 : BAD_COLOR;
    }

    inline int get_background()
    {
        return (get() != BAD_COLOR) ? get() / 16 : BAD_COLOR;
    }

    inline void set(int c)
    {
        if (is_good(c))
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), c);
    }

    inline void set(int a, int b)
    {
        set(a + b * 16);
    }

    inline void set(std::string a, std::string b)
    {
        set(stoc(a) + stoc(b) * 16);
    }

    inline void set_text(std::string a)
    {
        set(stoc(a), get_background());
    }

    inline void set_background(std::string b)
    {
        set(get_text(), stoc(b));
    }

    inline void reset()
    {
        set(DEFAULT_COLOR);
    }

    inline int invert(int c)
    {
        if (is_good(c)) {
            int a = c % 16;
            int b = c / 16;
            return b + a * 16;
        }
        else
            return BAD_COLOR;
    }

   inline std::ostream& reset(std::ostream& os) { reset();                return os; }
   inline std::ostream& black(std::ostream& os) { set_text(("k"));          return os; }
   inline std::ostream& blue(std::ostream& os) { set_text(("b"));          return os; }
   inline std::ostream& green(std::ostream& os) { set_text(("g"));          return os; }
   inline std::ostream& aqua(std::ostream& os) { set_text("a");          return os; }
   inline std::ostream& red(std::ostream& os) { set_text("r");          return os; }
   inline std::ostream& purple(std::ostream& os) { set_text("p");          return os; }
   inline std::ostream& yellow(std::ostream& os) { set_text("y");          return os; }
   inline std::ostream& white(std::ostream& os) { set_text("w");          return os; }
   inline std::ostream& grey(std::ostream& os) { set_text("e");          return os; }
   inline std::ostream& light_blue(std::ostream& os) { set_text("lb");         return os; }
   inline std::ostream& light_green(std::ostream& os) { set_text("lg");         return os; }
   inline std::ostream& light_aqua(std::ostream& os) { set_text("la");         return os; }
   inline std::ostream& light_red(std::ostream& os) { set_text("lr");         return os; }
   inline std::ostream& light_purple(std::ostream& os) { set_text("lp");         return os; }
   inline std::ostream& light_yellow(std::ostream& os) { set_text("ly");         return os; }
   inline std::ostream& bright_white(std::ostream& os) { set_text("bw");         return os; }
   inline std::ostream& on_black(std::ostream& os) { set_background("k");    return os; }
   inline std::ostream& on_blue(std::ostream& os) { set_background("b");    return os; }
   inline std::ostream& on_green(std::ostream& os) { set_background("g");    return os; }
   inline std::ostream& on_aqua(std::ostream& os) { set_background("a");    return os; }
   inline std::ostream& on_red(std::ostream& os) { set_background("r");    return os; }
   inline std::ostream& on_purple(std::ostream& os) { set_background("p");    return os; }
   inline std::ostream& on_yellow(std::ostream& os) { set_background("y");    return os; }
   inline std::ostream& on_white(std::ostream& os) { set_background("w");    return os; }
   inline std::ostream& on_grey(std::ostream& os) { set_background("e");    return os; }
   inline std::ostream& on_light_blue(std::ostream& os) { set_background("lb");   return os; }
   inline std::ostream& on_light_green(std::ostream& os) { set_background("lg");   return os; }
   inline std::ostream& on_light_aqua(std::ostream& os) { set_background("la");   return os; }
   inline std::ostream& on_light_red(std::ostream& os) { set_background("lr");   return os; }
   inline std::ostream& on_light_purple(std::ostream& os) { set_background("lp");   return os; }
   inline std::ostream& on_light_yellow(std::ostream& os) { set_background("ly");   return os; }
   inline std::ostream& on_bright_white(std::ostream& os) { set_background("bw");   return os; }
   inline std::ostream& black_on_black(std::ostream& os) { set("k", "k");   return os; }
   inline std::ostream& black_on_blue(std::ostream& os) { set("k", "b");   return os; }
   inline std::ostream& black_on_green(std::ostream& os) { set("k", "g");   return os; }
   inline std::ostream& black_on_aqua(std::ostream& os) { set("k", "a");   return os; }
   inline std::ostream& black_on_red(std::ostream& os) { set("k", "r");   return os; }
   inline std::ostream& black_on_purple(std::ostream& os) { set("k", "p");   return os; }
   inline std::ostream& black_on_yellow(std::ostream& os) { set("k", "y");   return os; }
   inline std::ostream& black_on_white(std::ostream& os) { set("k", "w");   return os; }
   inline std::ostream& black_on_grey(std::ostream& os) { set("k", "e");   return os; }
   inline std::ostream& black_on_light_blue(std::ostream& os) { set("k", "lb");  return os; }
   inline std::ostream& black_on_light_green(std::ostream& os) { set("k", "lg");  return os; }
   inline std::ostream& black_on_light_aqua(std::ostream& os) { set("k", "la");  return os; }
   inline std::ostream& black_on_light_red(std::ostream& os) { set("k", "lr");  return os; }
   inline std::ostream& black_on_light_purple(std::ostream& os) { set("k", "lp");  return os; }
   inline std::ostream& black_on_light_yellow(std::ostream& os) { set("k", "ly");  return os; }
   inline std::ostream& black_on_bright_white(std::ostream& os) { set("k", "bw");  return os; }
   inline std::ostream& blue_on_black(std::ostream& os) { set("b", "k");   return os; }
   inline std::ostream& blue_on_blue(std::ostream& os) { set("b", "b");   return os; }
   inline std::ostream& blue_on_green(std::ostream& os) { set("b", "g");   return os; }
   inline std::ostream& blue_on_aqua(std::ostream& os) { set("b", "a");   return os; }
   inline std::ostream& blue_on_red(std::ostream& os) { set("b", "r");   return os; }
   inline std::ostream& blue_on_purple(std::ostream& os) { set("b", "p");   return os; }
   inline std::ostream& blue_on_yellow(std::ostream& os) { set("b", "y");   return os; }
   inline std::ostream& blue_on_white(std::ostream& os) { set("b", "w");   return os; }
   inline std::ostream& blue_on_grey(std::ostream& os) { set("b", "e");   return os; }
   inline std::ostream& blue_on_light_blue(std::ostream& os) { set("b", "lb");  return os; }
   inline std::ostream& blue_on_light_green(std::ostream& os) { set("b", "lg");  return os; }
   inline std::ostream& blue_on_light_aqua(std::ostream& os) { set("b", "la");  return os; }
   inline std::ostream& blue_on_light_red(std::ostream& os) { set("b", "lr");  return os; }
   inline std::ostream& blue_on_light_purple(std::ostream& os) { set("b", "lp");  return os; }
   inline std::ostream& blue_on_light_yellow(std::ostream& os) { set("b", "ly");  return os; }
   inline std::ostream& blue_on_bright_white(std::ostream& os) { set("b", "bw");  return os; }
   inline std::ostream& green_on_black(std::ostream& os) { set("g", "k");   return os; }
   inline std::ostream& green_on_blue(std::ostream& os) { set("g", "b");   return os; }
   inline std::ostream& green_on_green(std::ostream& os) { set("g", "g");   return os; }
   inline std::ostream& green_on_aqua(std::ostream& os) { set("g", "a");   return os; }
   inline std::ostream& green_on_red(std::ostream& os) { set("g", "r");   return os; }
   inline std::ostream& green_on_purple(std::ostream& os) { set("g", "p");   return os; }
   inline std::ostream& green_on_yellow(std::ostream& os) { set("g", "y");   return os; }
   inline std::ostream& green_on_white(std::ostream& os) { set("g", "w");   return os; }
   inline std::ostream& green_on_grey(std::ostream& os) { set("g", "e");   return os; }
   inline std::ostream& green_on_light_blue(std::ostream& os) { set("g", "lb");  return os; }
   inline std::ostream& green_on_light_green(std::ostream& os) { set("g", "lg");  return os; }
   inline std::ostream& green_on_light_aqua(std::ostream& os) { set("g", "la");  return os; }
   inline std::ostream& green_on_light_red(std::ostream& os) { set("g", "lr");  return os; }
   inline std::ostream& green_on_light_purple(std::ostream& os) { set("g", "lp");  return os; }
   inline std::ostream& green_on_light_yellow(std::ostream& os) { set("g", "ly");  return os; }
   inline std::ostream& green_on_bright_white(std::ostream& os) { set("g", "bw");  return os; }
   inline std::ostream& aqua_on_black(std::ostream& os) { set("a", "k");   return os; }
   inline std::ostream& aqua_on_blue(std::ostream& os) { set("a", "b");   return os; }
   inline std::ostream& aqua_on_green(std::ostream& os) { set("a", "g");   return os; }
   inline std::ostream& aqua_on_aqua(std::ostream& os) { set("a", "a");   return os; }
   inline std::ostream& aqua_on_red(std::ostream& os) { set("a", "r");   return os; }
   inline std::ostream& aqua_on_purple(std::ostream& os) { set("a", "p");   return os; }
   inline std::ostream& aqua_on_yellow(std::ostream& os) { set("a", "y");   return os; }
   inline std::ostream& aqua_on_white(std::ostream& os) { set("a", "w");   return os; }
   inline std::ostream& aqua_on_grey(std::ostream& os) { set("a", "e");   return os; }
   inline std::ostream& aqua_on_light_blue(std::ostream& os) { set("a", "lb");  return os; }
   inline std::ostream& aqua_on_light_green(std::ostream& os) { set("a", "lg");  return os; }
   inline std::ostream& aqua_on_light_aqua(std::ostream& os) { set("a", "la");  return os; }
   inline std::ostream& aqua_on_light_red(std::ostream& os) { set("a", "lr");  return os; }
   inline std::ostream& aqua_on_light_purple(std::ostream& os) { set("a", "lp");  return os; }
   inline std::ostream& aqua_on_light_yellow(std::ostream& os) { set("a", "ly");  return os; }
   inline std::ostream& aqua_on_bright_white(std::ostream& os) { set("a", "bw");  return os; }
   inline std::ostream& red_on_black(std::ostream& os) { set("r", "k");   return os; }
   inline std::ostream& red_on_blue(std::ostream& os) { set("r", "b");   return os; }
   inline std::ostream& red_on_green(std::ostream& os) { set("r", "g");   return os; }
   inline std::ostream& red_on_aqua(std::ostream& os) { set("r", "a");   return os; }
   inline std::ostream& red_on_red(std::ostream& os) { set("r", "r");   return os; }
   inline std::ostream& red_on_purple(std::ostream& os) { set("r", "p");   return os; }
   inline std::ostream& red_on_yellow(std::ostream& os) { set("r", "y");   return os; }
   inline std::ostream& red_on_white(std::ostream& os) { set("r", "w");   return os; }
   inline std::ostream& red_on_grey(std::ostream& os) { set("r", "e");   return os; }
   inline std::ostream& red_on_light_blue(std::ostream& os) { set("r", "lb");  return os; }
   inline std::ostream& red_on_light_green(std::ostream& os) { set("r", "lg");  return os; }
   inline std::ostream& red_on_light_aqua(std::ostream& os) { set("r", "la");  return os; }
   inline std::ostream& red_on_light_red(std::ostream& os) { set("r", "lr");  return os; }
   inline std::ostream& red_on_light_purple(std::ostream& os) { set("r", "lp");  return os; }
   inline std::ostream& red_on_light_yellow(std::ostream& os) { set("r", "ly");  return os; }
   inline std::ostream& red_on_bright_white(std::ostream& os) { set("r", "bw");  return os; }
   inline std::ostream& purple_on_black(std::ostream& os) { set("p", "k");   return os; }
   inline std::ostream& purple_on_blue(std::ostream& os) { set("p", "b");   return os; }
   inline std::ostream& purple_on_green(std::ostream& os) { set("p", "g");   return os; }
   inline std::ostream& purple_on_aqua(std::ostream& os) { set("p", "a");   return os; }
   inline std::ostream& purple_on_red(std::ostream& os) { set("p", "r");   return os; }
   inline std::ostream& purple_on_purple(std::ostream& os) { set("p", "p");   return os; }
   inline std::ostream& purple_on_yellow(std::ostream& os) { set("p", "y");   return os; }
   inline std::ostream& purple_on_white(std::ostream& os) { set("p", "w");   return os; }
   inline std::ostream& purple_on_grey(std::ostream& os) { set("p", "e");   return os; }
   inline std::ostream& purple_on_light_blue(std::ostream& os) { set("p", "lb");  return os; }
   inline std::ostream& purple_on_light_green(std::ostream& os) { set("p", "lg");  return os; }
   inline std::ostream& purple_on_light_aqua(std::ostream& os) { set("p", "la");  return os; }
   inline std::ostream& purple_on_light_red(std::ostream& os) { set("p", "lr");  return os; }
   inline std::ostream& purple_on_light_purple(std::ostream& os) { set("p", "lp");  return os; }
   inline std::ostream& purple_on_light_yellow(std::ostream& os) { set("p", "ly");  return os; }
   inline std::ostream& purple_on_bright_white(std::ostream& os) { set("p", "bw");  return os; }
   inline std::ostream& yellow_on_black(std::ostream& os) { set("y", "k");   return os; }
   inline std::ostream& yellow_on_blue(std::ostream& os) { set("y", "b");   return os; }
   inline std::ostream& yellow_on_green(std::ostream& os) { set("y", "g");   return os; }
   inline std::ostream& yellow_on_aqua(std::ostream& os) { set("y", "a");   return os; }
   inline std::ostream& yellow_on_red(std::ostream& os) { set("y", "r");   return os; }
   inline std::ostream& yellow_on_purple(std::ostream& os) { set("y", "p");   return os; }
   inline std::ostream& yellow_on_yellow(std::ostream& os) { set("y", "y");   return os; }
   inline std::ostream& yellow_on_white(std::ostream& os) { set("y", "w");   return os; }
   inline std::ostream& yellow_on_grey(std::ostream& os) { set("y", "e");   return os; }
   inline std::ostream& yellow_on_light_blue(std::ostream& os) { set("y", "lb");  return os; }
   inline std::ostream& yellow_on_light_green(std::ostream& os) { set("y", "lg");  return os; }
   inline std::ostream& yellow_on_light_aqua(std::ostream& os) { set("y", "la");  return os; }
   inline std::ostream& yellow_on_light_red(std::ostream& os) { set("y", "lr");  return os; }
   inline std::ostream& yellow_on_light_purple(std::ostream& os) { set("y", "lp");  return os; }
   inline std::ostream& yellow_on_light_yellow(std::ostream& os) { set("y", "ly");  return os; }
   inline std::ostream& yellow_on_bright_white(std::ostream& os) { set("y", "bw");  return os; }
   inline std::ostream& white_on_black(std::ostream& os) { set("w", "k");   return os; }
   inline std::ostream& white_on_blue(std::ostream& os) { set("w", "b");   return os; }
   inline std::ostream& white_on_green(std::ostream& os) { set("w", "g");   return os; }
   inline std::ostream& white_on_aqua(std::ostream& os) { set("w", "a");   return os; }
   inline std::ostream& white_on_red(std::ostream& os) { set("w", "r");   return os; }
   inline std::ostream& white_on_purple(std::ostream& os) { set("w", "p");   return os; }
   inline std::ostream& white_on_yellow(std::ostream& os) { set("w", "y");   return os; }
   inline std::ostream& white_on_white(std::ostream& os) { set("w", "w");   return os; }
   inline std::ostream& white_on_grey(std::ostream& os) { set("w", "e");   return os; }
   inline std::ostream& white_on_light_blue(std::ostream& os) { set("w", "lb");  return os; }
   inline std::ostream& white_on_light_green(std::ostream& os) { set("w", "lg");  return os; }
   inline std::ostream& white_on_light_aqua(std::ostream& os) { set("w", "la");  return os; }
   inline std::ostream& white_on_light_red(std::ostream& os) { set("w", "lr");  return os; }
   inline std::ostream& white_on_light_purple(std::ostream& os) { set("w", "lp");  return os; }
   inline std::ostream& white_on_light_yellow(std::ostream& os) { set("w", "ly");  return os; }
   inline std::ostream& white_on_bright_white(std::ostream& os) { set("w", "bw");  return os; }
   inline std::ostream& grey_on_black(std::ostream& os) { set("e", "k");   return os; }
   inline std::ostream& grey_on_blue(std::ostream& os) { set("e", "b");   return os; }
   inline std::ostream& grey_on_green(std::ostream& os) { set("e", "g");   return os; }
   inline std::ostream& grey_on_aqua(std::ostream& os) { set("e", "a");   return os; }
   inline std::ostream& grey_on_red(std::ostream& os) { set("e", "r");   return os; }
   inline std::ostream& grey_on_purple(std::ostream& os) { set("e", "p");   return os; }
   inline std::ostream& grey_on_yellow(std::ostream& os) { set("e", "y");   return os; }
   inline std::ostream& grey_on_white(std::ostream& os) { set("e", "w");   return os; }
   inline std::ostream& grey_on_grey(std::ostream& os) { set("e", "e");   return os; }
   inline std::ostream& grey_on_light_blue(std::ostream& os) { set("e", "lb");  return os; }
   inline std::ostream& grey_on_light_green(std::ostream& os) { set("e", "lg");  return os; }
   inline std::ostream& grey_on_light_aqua(std::ostream& os) { set("e", "la");  return os; }
   inline std::ostream& grey_on_light_red(std::ostream& os) { set("e", "lr");  return os; }
   inline std::ostream& grey_on_light_purple(std::ostream& os) { set("e", "lp");  return os; }
   inline std::ostream& grey_on_light_yellow(std::ostream& os) { set("e", "ly");  return os; }
   inline std::ostream& grey_on_bright_white(std::ostream& os) { set("e", "bw");  return os; }
   inline std::ostream& light_blue_on_black(std::ostream& os) { set("lb", "k");  return os; }
   inline std::ostream& light_blue_on_blue(std::ostream& os) { set("lb", "b");  return os; }
   inline std::ostream& light_blue_on_green(std::ostream& os) { set("lb", "g");  return os; }
   inline std::ostream& light_blue_on_aqua(std::ostream& os) { set("lb", "a");  return os; }
   inline std::ostream& light_blue_on_red(std::ostream& os) { set("lb", "r");  return os; }
   inline std::ostream& light_blue_on_purple(std::ostream& os) { set("lb", "p");  return os; }
   inline std::ostream& light_blue_on_yellow(std::ostream& os) { set("lb", "y");  return os; }
   inline std::ostream& light_blue_on_white(std::ostream& os) { set("lb", "w");  return os; }
   inline std::ostream& light_blue_on_grey(std::ostream& os) { set("lb", "e");  return os; }
   inline std::ostream& light_blue_on_light_blue(std::ostream& os) { set("lb", "lb"); return os; }
   inline std::ostream& light_blue_on_light_green(std::ostream& os) { set("lb", "lg"); return os; }
   inline std::ostream& light_blue_on_light_aqua(std::ostream& os) { set("lb", "la"); return os; }
   inline std::ostream& light_blue_on_light_red(std::ostream& os) { set("lb", "lr"); return os; }
   inline std::ostream& light_blue_on_light_purple(std::ostream& os) { set("lb", "lp"); return os; }
   inline std::ostream& light_blue_on_light_yellow(std::ostream& os) { set("lb", "ly"); return os; }
   inline std::ostream& light_blue_on_bright_white(std::ostream& os) { set("lb", "bw"); return os; }
   inline std::ostream& light_green_on_black(std::ostream& os) { set("lg", "k");  return os; }
   inline std::ostream& light_green_on_blue(std::ostream& os) { set("lg", "b");  return os; }
   inline std::ostream& light_green_on_green(std::ostream& os) { set("lg", "g");  return os; }
   inline std::ostream& light_green_on_aqua(std::ostream& os) { set("lg", "a");  return os; }
   inline std::ostream& light_green_on_red(std::ostream& os) { set("lg", "r");  return os; }
   inline std::ostream& light_green_on_purple(std::ostream& os) { set("lg", "p");  return os; }
   inline std::ostream& light_green_on_yellow(std::ostream& os) { set("lg", "y");  return os; }
   inline std::ostream& light_green_on_white(std::ostream& os) { set("lg", "w");  return os; }
   inline std::ostream& light_green_on_grey(std::ostream& os) { set("lg", "e");  return os; }
   inline std::ostream& light_green_on_light_blue(std::ostream& os) { set("lg", "lb"); return os; }
   inline std::ostream& light_green_on_light_green(std::ostream& os) { set("lg", "lg"); return os; }
   inline std::ostream& light_green_on_light_aqua(std::ostream& os) { set("lg", "la"); return os; }
   inline std::ostream& light_green_on_light_red(std::ostream& os) { set("lg", "lr"); return os; }
   inline std::ostream& light_green_on_light_purple(std::ostream& os) { set("lg", "lp"); return os; }
   inline std::ostream& light_green_on_light_yellow(std::ostream& os) { set("lg", "ly"); return os; }
   inline std::ostream& light_green_on_bright_white(std::ostream& os) { set("lg", "bw"); return os; }
   inline std::ostream& light_aqua_on_black(std::ostream& os) { set("la", "k");  return os; }
   inline std::ostream& light_aqua_on_blue(std::ostream& os) { set("la", "b");  return os; }
   inline std::ostream& light_aqua_on_green(std::ostream& os) { set("la", "g");  return os; }
   inline std::ostream& light_aqua_on_aqua(std::ostream& os) { set("la", "a");  return os; }
   inline std::ostream& light_aqua_on_red(std::ostream& os) { set("la", "r");  return os; }
   inline std::ostream& light_aqua_on_purple(std::ostream& os) { set("la", "p");  return os; }
   inline std::ostream& light_aqua_on_yellow(std::ostream& os) { set("la", "y");  return os; }
   inline std::ostream& light_aqua_on_white(std::ostream& os) { set("la", "w");  return os; }
   inline std::ostream& light_aqua_on_grey(std::ostream& os) { set("la", "e");  return os; }
   inline std::ostream& light_aqua_on_light_blue(std::ostream& os) { set("la", "lb"); return os; }
   inline std::ostream& light_aqua_on_light_green(std::ostream& os) { set("la", "lg"); return os; }
   inline std::ostream& light_aqua_on_light_aqua(std::ostream& os) { set("la", "la"); return os; }
   inline std::ostream& light_aqua_on_light_red(std::ostream& os) { set("la", "lr"); return os; }
   inline std::ostream& light_aqua_on_light_purple(std::ostream& os) { set("la", "lp"); return os; }
   inline std::ostream& light_aqua_on_light_yellow(std::ostream& os) { set("la", "ly"); return os; }
   inline std::ostream& light_aqua_on_bright_white(std::ostream& os) { set("la", "bw"); return os; }
   inline std::ostream& light_red_on_black(std::ostream& os) { set("lr", "k");  return os; }
   inline std::ostream& light_red_on_blue(std::ostream& os) { set("lr", "b");  return os; }
   inline std::ostream& light_red_on_green(std::ostream& os) { set("lr", "g");  return os; }
   inline std::ostream& light_red_on_aqua(std::ostream& os) { set("lr", "a");  return os; }
   inline std::ostream& light_red_on_red(std::ostream& os) { set("lr", "r");  return os; }
   inline std::ostream& light_red_on_purple(std::ostream& os) { set("lr", "p");  return os; }
   inline std::ostream& light_red_on_yellow(std::ostream& os) { set("lr", "y");  return os; }
   inline std::ostream& light_red_on_white(std::ostream& os) { set("lr", "w");  return os; }
   inline std::ostream& light_red_on_grey(std::ostream& os) { set("lr", "e");  return os; }
   inline std::ostream& light_red_on_light_blue(std::ostream& os) { set("lr", "lb"); return os; }
   inline std::ostream& light_red_on_light_green(std::ostream& os) { set("lr", "lg"); return os; }
   inline std::ostream& light_red_on_light_aqua(std::ostream& os) { set("lr", "la"); return os; }
   inline std::ostream& light_red_on_light_red(std::ostream& os) { set("lr", "lr"); return os; }
   inline std::ostream& light_red_on_light_purple(std::ostream& os) { set("lr", "lp"); return os; }
   inline std::ostream& light_red_on_light_yellow(std::ostream& os) { set("lr", "ly"); return os; }
   inline std::ostream& light_red_on_bright_white(std::ostream& os) { set("lr", "bw"); return os; }
   inline std::ostream& light_purple_on_black(std::ostream& os) { set("lp", "k");  return os; }
   inline std::ostream& light_purple_on_blue(std::ostream& os) { set("lp", "b");  return os; }
   inline std::ostream& light_purple_on_green(std::ostream& os) { set("lp", "g");  return os; }
   inline std::ostream& light_purple_on_aqua(std::ostream& os) { set("lp", "a");  return os; }
   inline std::ostream& light_purple_on_red(std::ostream& os) { set("lp", "r");  return os; }
   inline std::ostream& light_purple_on_purple(std::ostream& os) { set("lp", "p");  return os; }
   inline std::ostream& light_purple_on_yellow(std::ostream& os) { set("lp", "y");  return os; }
   inline std::ostream& light_purple_on_white(std::ostream& os) { set("lp", "w");  return os; }
   inline std::ostream& light_purple_on_grey(std::ostream& os) { set("lp", "e");  return os; }
   inline std::ostream& light_purple_on_light_blue(std::ostream& os) { set("lp", "lb"); return os; }
   inline std::ostream& light_purple_on_light_green(std::ostream& os) { set("lp", "lg"); return os; }
   inline std::ostream& light_purple_on_light_aqua(std::ostream& os) { set("lp", "la"); return os; }
   inline std::ostream& light_purple_on_light_red(std::ostream& os) { set("lp", "lr"); return os; }
   inline std::ostream& light_purple_on_light_purple(std::ostream& os) { set("lp", "lp"); return os; }
   inline std::ostream& light_purple_on_light_yellow(std::ostream& os) { set("lp", "ly"); return os; }
   inline std::ostream& light_purple_on_bright_white(std::ostream& os) { set("lp", "bw"); return os; }
   inline std::ostream& light_yellow_on_black(std::ostream& os) { set("ly", "k");  return os; }
   inline std::ostream& light_yellow_on_blue(std::ostream& os) { set("ly", "b");  return os; }
   inline std::ostream& light_yellow_on_green(std::ostream& os) { set("ly", "g");  return os; }
   inline std::ostream& light_yellow_on_aqua(std::ostream& os) { set("ly", "a");  return os; }
   inline std::ostream& light_yellow_on_red(std::ostream& os) { set("ly", "r");  return os; }
   inline std::ostream& light_yellow_on_purple(std::ostream& os) { set("ly", "p");  return os; }
   inline std::ostream& light_yellow_on_yellow(std::ostream& os) { set(("ly"), ("y"));  return os; }
   inline std::ostream& light_yellow_on_white(std::ostream& os) { set(("ly"), ("w"));  return os; }
   inline std::ostream& light_yellow_on_grey(std::ostream& os) { set(("ly"), ("e"));  return os; }
   inline std::ostream& light_yellow_on_light_blue(std::ostream& os) { set(("ly"), ("lb")); return os; }
   inline std::ostream& light_yellow_on_light_green(std::ostream& os) { set(("ly"), ("lg")); return os; }
   inline std::ostream& light_yellow_on_light_aqua(std::ostream& os) { set(("ly"), ("la")); return os; }
   inline std::ostream& light_yellow_on_light_red(std::ostream& os) { set(("ly"), ("lr")); return os; }
   inline std::ostream& light_yellow_on_light_purple(std::ostream& os) { set(("ly"), ("lp")); return os; }
   inline std::ostream& light_yellow_on_light_yellow(std::ostream& os) { set(("ly"), ("ly")); return os; }
   inline std::ostream& light_yellow_on_bright_white(std::ostream& os) { set(("ly"), ("bw")); return os; }
   inline std::ostream& bright_white_on_black(std::ostream& os) { set(("bw"), ("k"));  return os; }
   inline std::ostream& bright_white_on_blue(std::ostream& os) { set(("bw"), ("b"));  return os; }
   inline std::ostream& bright_white_on_green(std::ostream& os) { set(("bw"), ("g"));  return os; }
   inline std::ostream& bright_white_on_aqua(std::ostream& os) { set(("bw"), ("a"));  return os; }
   inline std::ostream& bright_white_on_red(std::ostream& os) { set(("bw"), ("r"));  return os; }
   inline std::ostream& bright_white_on_purple(std::ostream& os) { set(("bw"), ("p"));  return os; }
   inline std::ostream& bright_white_on_yellow(std::ostream& os) { set(("bw"), ("y"));  return os; }
   inline std::ostream& bright_white_on_white(std::ostream& os) { set(("bw"), ("w"));  return os; }
   inline std::ostream& bright_white_on_grey(std::ostream& os) { set(("bw"), ("e"));  return os; }
   inline std::ostream& bright_white_on_light_blue(std::ostream& os) { set(("bw"), ("lb")); return os; }
   inline std::ostream& bright_white_on_light_green(std::ostream& os) { set(("bw"), ("lg")); return os; }
   inline std::ostream& bright_white_on_light_aqua(std::ostream& os) { set(("bw"), ("la")); return os; }
   inline std::ostream& bright_white_on_light_red(std::ostream& os) { set(("bw"), ("lr")); return os; }
   inline std::ostream& bright_white_on_light_purple(std::ostream& os) { set(("bw"), ("lp")); return os; }
   inline std::ostream& bright_white_on_light_yellow(std::ostream& os) { set(("bw"), ("ly")); return os; }
   inline std::ostream& bright_white_on_bright_white(std::ostream& os) { set(("bw"), ("bw")); return os; }
}