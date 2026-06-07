#include "CarouselConfig.h"

#include "config_helper.h"

#include "clocktile/ClockTile.h"
#include "cryptotile/CryptoSource.h"
#include "cryptotile/CryptoTile.h"
#include "fxtile/FxSource.h"
#include "fxtile/FxTile.h"
#include "launchtile/LaunchSource.h"
#include "launchtile/LaunchTile.h"
#include "stocktile/StockSource.h"
#include "stocktile/StockTile.h"
#include "suntile/SunSource.h"
#include "suntile/SunTile.h"
#include "weathertile/WeatherSource.h"
#include "weathertile/WeatherTile.h"

// ============================================================================
// This is the authoritative tile ribbon. Reorder / comment-out tiles here; the
// window of 5 visible orbs slides across this list one orb at a time.
// API keys + feature toggles live in firmware/config/config.h.
// ============================================================================
void buildCarousel(Carousel &carousel) {
    // --- World clocks (POSIX TZ, no API) ---
    carousel.addTile(new ClockTile("Los Angeles", "PST8PDT,M3.2.0,M11.1.0"));
    carousel.addTile(new ClockTile("New York", "EST5EDT,M3.2.0,M11.1.0"));
    carousel.addTile(new ClockTile("Paris", "CET-1CEST,M3.5.0,M10.5.0/3"));

    // --- Equities / ETFs / index proxies (TwelveData) ---
#ifdef STOCK_TICKER_LIST
    StockSource *stocks = new StockSource(STOCK_TICKER_LIST);
    carousel.addSource(stocks);
    for (size_t i = 0; i < stocks->count(); i++) {
        carousel.addTile(new StockTile(stocks, stocks->symbolAt(i)));
    }
#endif

    // --- Weather (Visual Crossing, °C) ---
    WeatherSource *sjWeather = new WeatherSource("San Jose, CA", "metric");
    WeatherSource *mtlWeather = new WeatherSource("Montreal, QC", "metric");
    carousel.addSource(sjWeather);
    carousel.addSource(mtlWeather);
    carousel.addTile(new WeatherTile(sjWeather, "San Jose"));
    carousel.addTile(new WeatherTile(mtlWeather, "Montreal"));

    // --- Crypto (CoinGecko, keyless) ---
#ifdef ENABLE_CRYPTO
    CryptoSource *crypto = new CryptoSource(ENABLE_CRYPTO);
    carousel.addSource(crypto);
    carousel.addTile(new CryptoTile(crypto, "bitcoin", "BTC"));
    carousel.addTile(new CryptoTile(crypto, "ethereum", "ETH"));
#endif

    // --- FX (open.er-api.com, keyless) ---
#ifdef ENABLE_FX
    FxSource *fx = new FxSource(ENABLE_FX);
    carousel.addSource(fx);
    carousel.addTile(new FxTile(fx, "EUR"));
    carousel.addTile(new FxTile(fx, "CAD"));
#endif

    // --- Sun / Moon (sunrise-sunset.org, keyless) ---
#ifdef ENABLE_SUN
    // San Jose, CA coordinates; display times in US Pacific.
    SunSource *sun = new SunSource(37.3382, -121.8863);
    carousel.addSource(sun);
    carousel.addTile(new SunTile(sun, "PST8PDT,M3.2.0,M11.1.0", "San Jose"));
#endif

    // --- Next rocket launch (Launch Library 2, keyless) ---
#ifdef ENABLE_LAUNCH
    LaunchSource *launch = new LaunchSource();
    carousel.addSource(launch);
    carousel.addTile(new LaunchTile(launch));
#endif
}
