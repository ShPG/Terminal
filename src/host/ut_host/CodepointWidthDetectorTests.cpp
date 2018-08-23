/********************************************************
*                                                       *
*   Copyright (C) Microsoft. All rights reserved.       *
*                                                       *
********************************************************/

#include "precomp.h"
#include "WexTestClass.h"
#include "..\..\inc\consoletaeftemplates.hpp"
#include "CommonState.hpp"

#include "../CodepointWidthDetector.hpp"

using namespace WEX::Logging;

static const std::wstring emoji = L"\xD83E\xDD22"; // U+1F922 nauseated face

// codepoint and utf16 encoded string
static const std::vector<std::tuple<unsigned int, std::wstring, CodepointWidth>> testData =
{
    { 0x7, L"\a", CodepointWidth::Narrow }, // BEL
    { 0x20, L" ", CodepointWidth::Narrow },
    { 0x39, L"9", CodepointWidth::Narrow },
    { 0x414, L"\x414", CodepointWidth::Ambiguous }, // U+0414 cyrillic capital de
    { 0x1104, L"\x1104", CodepointWidth::Wide }, // U+1104 hangul choseong ssangtikeut
    { 0x306A, L"\x306A", CodepointWidth::Wide }, // U+306A hiragana na
    { 0x30CA, L"\x30CA", CodepointWidth::Wide }, // U+30CA katakana na
    { 0x72D7, L"\x72D7", CodepointWidth::Wide }, // U+72D7
    { 0x1F47E, L"\xD83D\xDC7E", CodepointWidth::Wide }, // U+1F47E alien monster
    { 0x1F51C, L"\xD83D\xDD1C", CodepointWidth::Wide } // U+1F51C SOON
};

class CodepointWidthDetectorTests
{
    TEST_CLASS(CodepointWidthDetectorTests);


    TEST_METHOD(CodepointWidthDetectDefersMapPopulation)
    {
        const CodepointWidthDetector widthDetector;
        VERIFY_IS_TRUE(widthDetector._map.empty());
        widthDetector.IsWide(UNICODE_SPACE);
        VERIFY_IS_TRUE(widthDetector._map.empty());
        // now force checking
        widthDetector.GetWidth(emoji);
        VERIFY_IS_FALSE(widthDetector._map.empty());
    }

    TEST_METHOD(CanLookUpEmoji)
    {
        const CodepointWidthDetector widthDetector;
        VERIFY_IS_TRUE(widthDetector.IsWide(emoji));
    }

    TEST_METHOD(TestUnicodeRangeCompare)
    {
        const CodepointWidthDetector::UnicodeRangeCompare compare;
        // test comparing 2 search terms
        CodepointWidthDetector::UnicodeRange a{ 0x10 };
        CodepointWidthDetector::UnicodeRange b{ 0x15 };
        VERIFY_IS_TRUE(static_cast<bool>(compare(a, b)));
    }

    TEST_METHOD(CanExtractCodepoint)
    {
        const CodepointWidthDetector widthDetector;
        for (const auto& data : testData)
        {
            const auto& expected = std::get<0>(data);
            const auto& wstr = std::get<1>(data);
            const auto result = widthDetector._extractCodepoint({ wstr.c_str(), wstr.size() });
            VERIFY_ARE_EQUAL(result, expected);
        }
    }

    TEST_METHOD(CanGetWidths)
    {
        const CodepointWidthDetector widthDetector;
        for (const auto& data : testData)
        {
            const auto& expected = std::get<2>(data);
            const auto& wstr = std::get<1>(data);
            const auto result = widthDetector.GetWidth({ wstr.c_str(), wstr.size() });
            VERIFY_ARE_EQUAL(result, expected);
        }
    }

};
