// string_fmt.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _TEST_UNIT_UMLS_STRING_FMT_HPP_
#define _TEST_UNIT_UMLS_STRING_FMT_HPP_
#include <gtest/gtest.h>
#include <umls/format.hpp>

namespace mjx {
    namespace test {
        TEST(string_fmt, formattable_empty) {
            EXPECT_FALSE(::mjx::is_formattable(L""));
            EXPECT_FALSE(::mjx::is_formattable(L""));
            EXPECT_FALSE(::mjx::is_formattable(L""));
            EXPECT_FALSE(::mjx::is_formattable(L""));
            EXPECT_FALSE(::mjx::is_formattable(L""));
            EXPECT_FALSE(::mjx::is_formattable(L""));
            EXPECT_FALSE(::mjx::is_formattable(L""));
            EXPECT_FALSE(::mjx::is_formattable(L""));
            EXPECT_FALSE(::mjx::is_formattable(L""));
            EXPECT_FALSE(::mjx::is_formattable(L""));
        }

        TEST(string_fmt, formattable_no_format_specifiers) {
            EXPECT_FALSE(::mjx::is_formattable(
                L"The mysterious old house creaked in the silence of the night."));
            EXPECT_FALSE(::mjx::is_formattable(
                L"A playful breeze danced through the colorful autumn leaves."));
            EXPECT_FALSE(::mjx::is_formattable(
                L"The scientist eagerly examined the results of the groundbreaking experiment."));
            EXPECT_FALSE(::mjx::is_formattable(
                L"Laughter echoed through the cheerful gathering of friends at the beach."));
            EXPECT_FALSE(::mjx::is_formattable(
                L"In the bustling city, neon lights painted the streets with vibrant colors."));
            EXPECT_FALSE(::mjx::is_formattable(
                L"Lost in thought, she gazed at the starry sky, contemplating life's mysteries."));
            EXPECT_FALSE(::mjx::is_formattable(
                L"The aroma of freshly baked cookies filled the cozy kitchen."));
            EXPECT_FALSE(::mjx::is_formattable(
                L"The adventurer discovered a hidden cave behind the cascading waterfall."));
            EXPECT_FALSE(::mjx::is_formattable(
                L"As dawn broke, the sleepy town slowly came to life."));
            EXPECT_FALSE(::mjx::is_formattable(
                L"A gentle rain tapped on the window, creating a soothing lullaby."));
        }

        TEST(string_fmt, formattable_broken_format_specifiers) {
            EXPECT_FALSE(::mjx::is_formattable(
                L"The {% 1} fox jumped over the lazy {% x} in the {% } moonlight."));
            EXPECT_FALSE(::mjx::is_formattable(
                L"A {%51 } magician revealed {%xy} tricks to the awe-struck {%%} audience."));
            EXPECT_FALSE(::mjx::is_formattable(
                L"In the mysterious {%z1} forest, a {%1000} adventure awaited {% 10} explorers."));
            EXPECT_FALSE(::mjx::is_formattable(
                L"As the clock struck {% 10}, the ghostly figure appeared at { %10} haunted mansion."));
            EXPECT_FALSE(::mjx::is_formattable(
                L"The detective carefully examined the {%} crime scene, searching for {%1002} clues."));
            EXPECT_FALSE(::mjx::is_formattable(
                L"Lost in the pages of {% 10} book, she discovered {%x10} fascinating worlds."));
            EXPECT_FALSE(::mjx::is_formattable(
                L"The inventor showcased {% % % % %} latest invention at the {%FF} technology expo."));
            EXPECT_FALSE(::mjx::is_formattable(
                L"During the {%x5y} eclipse, a rare celestial event captured {% 1} imaginations."));
            EXPECT_FALSE(::mjx::is_formattable(
                L"At the {%xy %} laboratory, scientists conducted {%10%} experiments with {%%1} precision."));
            EXPECT_FALSE(::mjx::is_formattable(
                L"The {% 1} chef prepared a {%xy} dish with {% % %} secret ingredients."));
        }

        TEST(string_fmt, formattable_valid_format_specifiers) {
            EXPECT_TRUE(::mjx::is_formattable(
                L"The {%2} friends shared {%4} pizzas at the {%10} gathering."));
            EXPECT_TRUE(::mjx::is_formattable(
                L"In the {%31} chapters of the book, the author explores {%500} different themes."));
            EXPECT_TRUE(::mjx::is_formattable(
                L"The {%0} team won the championship after a series of {%28} intense matches."));
            EXPECT_TRUE(::mjx::is_formattable(
                L"The {%999} experiment yielded {%45} unexpected results."));
            EXPECT_TRUE(::mjx::is_formattable(
                L"The {%3} musicians played {%02} instruments during the live concert."));
            EXPECT_TRUE(::mjx::is_formattable(
                L"On the {%1} day of the trip, we visited {%50} historical landmarks."));
            EXPECT_TRUE(::mjx::is_formattable(
                L"The {%401} actors delivered outstanding performances in the {%2} acts."));
            EXPECT_TRUE(::mjx::is_formattable(
                L"The {%390} courses covered {%55} diverse topics in the curriculum."));
            EXPECT_TRUE(::mjx::is_formattable(
                L"{%2} artists collaborated to create a {%14} vibrant mural in the city."));
            EXPECT_TRUE(::mjx::is_formattable(
                L"The {%1} project required {%3} months of dedicated work by {%4} skilled engineers."));
        }

        TEST(string_fmt, formattable_mixed_format_specifiers) {
            EXPECT_TRUE(::mjx::is_formattable(L"The {%2} team scored {%x} goals in the {% 10} match."));
            EXPECT_TRUE(::mjx::is_formattable(
                L"During the {%3} sessions, the speaker covered {%1_0} topics, leaving the audience {%2}."));
            EXPECT_TRUE(::mjx::is_formattable(
                L"{%4} students participated in the {%y} science fair, showcasing { %1} innovative projects."));
            EXPECT_TRUE(::mjx::is_formattable(
                L"The {%x} experiment had {%3} unexpected outcomes, surprising both {%2 } researchers."));
            EXPECT_TRUE(::mjx::is_formattable(L"In the {%5} chapters of the novel, characters encountered {% 2} "
                L"challenges and overcame{% 789} obstacles."));
            EXPECT_TRUE(::mjx::is_formattable(
                L"The {%2} chefs prepared {%F01} dishes for the {%99} guests, receiving {%10%} compliments."));
            EXPECT_TRUE(::mjx::is_formattable(
                L"The {%4} actors performed {%3} scenes, each portraying {%5 1} different emotions."));
            EXPECT_TRUE(::mjx::is_formattable(
                L"During the {%9} days of the festival, attendees enjoyed {%3000} concerts "
                L"featuring { % 10 } talented musicians."));
            EXPECT_TRUE(::mjx::is_formattable(
                L"{%001} volunteers worked together to clean up {%5 } parks, creating a {%10%} positive impact."));
            EXPECT_TRUE(::mjx::is_formattable(
                L"The {%x} project required {%199} hours of focused work from {%%3} dedicated team members."));
        }

        TEST(string_fmt, format_empty) {
            EXPECT_EQ(::mjx::format_string(L"", format_args{}), L"");
            EXPECT_EQ(::mjx::format_string(L"", format_args{}), L"");
            EXPECT_EQ(::mjx::format_string(L"", format_args{}), L"");
            EXPECT_EQ(::mjx::format_string(L"", format_args{}), L"");
            EXPECT_EQ(::mjx::format_string(L"", format_args{}), L"");
            EXPECT_EQ(::mjx::format_string(L"", format_args{}), L"");
            EXPECT_EQ(::mjx::format_string(L"", format_args{}), L"");
            EXPECT_EQ(::mjx::format_string(L"", format_args{}), L"");
            EXPECT_EQ(::mjx::format_string(L"", format_args{}), L"");
            EXPECT_EQ(::mjx::format_string(L"", format_args{}), L"");
        }

        TEST(string_fmt, format_invalid) {
            EXPECT_EQ(::mjx::format_string(L"There are {%0} apples in the basket.",
                ::mjx::make_format_args()), L""
            );
            EXPECT_EQ(::mjx::format_string(L"The {%0} is {%1} degrees.",
                ::mjx::make_format_args(L"temperature")), L""
            );
            EXPECT_EQ(::mjx::format_string(L"The {%0} will take place on {%3}.",
                ::mjx::make_format_args(L"event", L"Saturday")), L""
            );
            EXPECT_EQ(::mjx::format_string(L"The book costs {%1} dollars.",
                ::mjx::make_format_args(L"10")), L""
            );
            EXPECT_EQ(::mjx::format_string(L"I have {%0} pets, including {%3} cats.",
                ::mjx::make_format_args(L"5", L"3")), L""
            );
            EXPECT_EQ(::mjx::format_string(L"The car's top speed is {%0} {%1}.",
                ::mjx::make_format_args(L"240")), L""
            );
            EXPECT_EQ(::mjx::format_string(L"He scored {%12} points in the game.",
                ::mjx::make_format_args(L"100", L"50", L"25")), L""
            );
            EXPECT_EQ(::mjx::format_string(L"The project has {%2} phases.",
                ::mjx::make_format_args(L"many", L"a lot of")), L""
            );
            EXPECT_EQ(::mjx::format_string(L"She bought {%1} pairs of shoes for {%50} dollars.",
                ::mjx::make_format_args(L"one", L"100", L"300")), L""
            );
            EXPECT_EQ(::mjx::format_string(L"The recipe calls for {%3} cups of sugar.",
                ::mjx::make_format_args(L"1", L"2", L"3")), L""
            );
        }

        TEST(string_fmt, format_valid) {
            EXPECT_EQ(::mjx::format_string(
                L"Welcome to the {%0} conference, where {%1} experts will share insights on {%2} topics.",
                ::mjx::make_format_args(L"International AI", L"5", L"cutting-edge")),
                L"Welcome to the International AI conference, where 5 experts will share insights "
                L"on cutting-edge topics."
            );
            EXPECT_EQ(::mjx::format_string(
                L"The recipe calls for {%0} cups of flour, {%1} teaspoon of salt, and {%2} eggs.",
                ::mjx::make_format_args(L"3", L"1", L"fresh")),
                L"The recipe calls for 3 cups of flour, 1 teaspoon of salt, and fresh eggs."
            );
            EXPECT_EQ(::mjx::format_string(
                L"{%0} employees attended the {%1} training session to enhance their {%2} skills.",
                ::mjx::make_format_args(L"120", L"workplace", L"communication")),
                L"120 employees attended the workplace training session to enhance their communication skills."
            );
            EXPECT_EQ(::mjx::format_string(
                L"Unlock the power of {%0} and embrace {%1} today!",
                ::mjx::make_format_args(L"knowledge", L"success")),
                L"Unlock the power of knowledge and embrace success today!"
            );
            EXPECT_EQ(::mjx::format_string(
                L"The {%0} participants in the marathon completed the race in {%1} hours on average.",
                ::mjx::make_format_args(L"500", L"2.5")),
                L"The 500 participants in the marathon completed the race in 2.5 hours on average."
            );
            EXPECT_EQ(::mjx::format_string(
                L"The software update includes {%0} new features and addresses {%1} reported bugs.",
                ::mjx::make_format_args(L"10", L"15")),
                L"The software update includes 10 new features and addresses 15 reported bugs."
            );
            EXPECT_EQ(::mjx::format_string(
                L"{%0} students received {%1} scholarships, allowing them to pursue {%2} degrees.",
                ::mjx::make_format_args(L"50", L"20", L"various")),
                L"50 students received 20 scholarships, allowing them to pursue various degrees."
            );
            EXPECT_EQ(::mjx::format_string(
                L"The art exhibition features {%0} paintings created by {%1} renowned artists.",
                ::mjx::make_format_args(L"30", L"10")),
                L"The art exhibition features 30 paintings created by 10 renowned artists."
            );
            EXPECT_EQ(::mjx::format_string(
                L"Attendees at the {%0} concert enjoyed performances by {%1} talented musicians.",
                ::mjx::make_format_args(L"1000", L"5")),
                L"Attendees at the 1000 concert enjoyed performances by 5 talented musicians."
            );
            EXPECT_EQ(::mjx::format_string(
                L"{%0} volunteers dedicated {%1} hours to clean up {%2} local parks, making a positive impact.",
                ::mjx::make_format_args(L"3", L"50", L"6")),
                L"3 volunteers dedicated 50 hours to clean up 6 local parks, making a positive impact."
            );
        }
    } // namespace test
} // namespace mjx

#endif // _TEST_UNIT_UMLS_STRING_FMT_HPP_