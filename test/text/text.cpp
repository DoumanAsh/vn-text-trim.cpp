#include <boost/test/unit_test.hpp>

#include "text/text.hpp"

BOOST_AUTO_TEST_CASE(should_clean_text) {
    const std::wstring expected_result(L"甘いものは別腹と言いますから。私も見なかったこと作戦で食べちゃいます");
    const std::wstring str(L"「甘いものは別腹と言いますから。私も見なかったこと作戦で食べちゃいます」");

    text::Cleaner cleaner;
    cleaner.emplace_back(std::wregex(L"^[「（](.+)[」 ）]$"), L"$1");

    const auto result = cleaner.clean(str);

    BOOST_REQUIRE(result.has_value());
    BOOST_REQUIRE(*result == expected_result);
}

BOOST_AUTO_TEST_CASE(should_clean_reps_text) {
    const std::wstring expected_result(L"御館様の想定通り、信濃勢は徹底抗戦の構えを見せた。");
    const std::wstring str(L"御館様の想定通り、<color=#ffffff24>信濃勢は御館様の想定通り</color>、信濃勢は徹底抗戦の御館様の想定通り、信濃勢は徹底抗戦の構えを見御館様の想定通り、信濃勢は徹底抗戦の構えを見せた。");

    text::Cleaner cleaner;
    cleaner.emplace_back(std::wregex(L"<[^>]+>"), L"")
           .emplace_back(std::wregex(L".*(.+)\\1+"), L"$1");

    const auto result = cleaner.clean(str);
    BOOST_REQUIRE(result.has_value());
    BOOST_REQUIRE(*result == expected_result);
}
