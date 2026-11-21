#pragma once

#include <memory>

#include "TouhouBase.hpp"

//! @brief Get an Touhou game instance.
std::unique_ptr<TouhouBase> initializeTouhouGame(bool initLogSilence);
