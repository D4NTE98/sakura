<h1 align="center">SAKURA</h1>

<p align="center">
  <b>v2.0.1</b><br>
  Refreshed by D4NTE
</p>

<div align="center">
  <img src="https://img.shields.io/badge/Version-v2.0.1-ff3d7f">
  <img src="https://img.shields.io/badge/License-MIT-green">
  <img src="https://img.shields.io/badge/Language-C%2B%2B-red">
  <img src="https://img.shields.io/badge/Lua-Supported-2c2d72">
  <img src="https://img.shields.io/badge/Visual%20Studio-2019%20%7C%202022-5c2d91">
</div>

<h4 align="center">
  Sakura is a free and public Counter-Strike 1.6 project written in <b>C++</b>, featuring Legit/Rage aiming systems, visuals, movement features, Lua scripting and a customizable ImGui interface.
</h4>

<p align="center">
  <a href="#key-features">Key Features</a> •
  <a href="#whats-new-in-v200">v2.0.0</a> •
  <a href="#whats-new-in-v201">v2.0.1</a> •
  <a href="#lua-scripting">Lua Scripting</a> •
  <a href="#how-to-build">How To Build</a> •
  <a href="#known-issues">Known Issues</a> •
  <a href="#credits">Credits</a> •
  <a href="#license">License</a>
</p>

---

## About This Refresh

Sakura v2.x is a refreshed continuation of the original Sakura codebase.

The goal of the v2 branch is to modernize the interface, improve runtime stability, make Lua scripting safer to use, repair legacy systems that were no longer behaving correctly, and improve the overall consistency of the project without removing the character of the original Sakura release.

The v2 refresh is maintained by **D4NTE**.

---

## Key Features

- **Legit Aimbot** with configurable weapon-based settings, FOV, smoothing, hitbox selection and Perfect Silent support.
- **Rage Aimbot** with multiple target-selection modes, penetration support, No Recoil, No Spread and additional rage-oriented features.
- **Backtrack / Fake Latency** with reconstructed historical player positions and visual backtrack rendering.
- **Backtrack-aware aiming in v2.0.1**, allowing both Legit and Rage Aimbot to evaluate historical records when Backtrack is enabled.
- **Visuals** including ESP, Chams, Light, Skeletons, Hitboxes, Radar, Snaplines, Bullet Traces, Death Marks, Crosshair and additional world/player rendering options.
- **Movement features** including Bunny Hop, Strafe, Ground Strafe, Fast Run and Jump Bug.
- **Lua scripting** using LuaBridge with callbacks, game/player helpers, ImGui integration, notifications and runtime script reloading.
- **Integrated Lua Workspace** directly inside the Sakura menu.
- **Modernized notification system** with card-style toasts, fade animations, slide transitions and progress indicators.
- **Customizable ImGui interface** with configurable colors and refreshed controls.
- **Configuration system** for saving and reloading Sakura settings.

---

## What's New in v2.0.0

Version 2.0.0 is the main refresh release and introduces the largest UI, Lua and stability update to the project.

### Interface Refresh

- Completely refreshed Sakura menu layout.
- Increased menu size and spacing for better readability.
- New dark visual style with rounded panels and cleaner separation between navigation and content.
- Redesigned tabs, sub-tabs, buttons, checkboxes and interactive states.
- Improved padding, spacing, borders and hover feedback throughout the interface.
- Added a cleaner left-side navigation panel and dedicated main content area.
- Reworked header presentation.
- Removed the old menu logo presentation in favor of a clean **SAKURA** text header.
- Added **v2.0** below the Sakura title.
- Added footer branding:
  - **Refreshed by D4NTE** on the left.
  - **v2.0** on the right.
- Improved overall ImGui styling, including window, frame, popup, scrollbar and grab rounding.
- Updated menu color handling while retaining user-customizable color settings.

### Lua Workspace

- Added a dedicated **Lua** tab to the main Sakura menu.
- Added an integrated Lua Workspace instead of relying on the old separate Lua menu window.
- Added loaded script counter.
- Added **Reload Lua** directly inside the main menu.
- Added script selection for scripts that register menu-render callbacks.
- Added clear empty-state information when no compatible Lua UI callback is available.
- Added safer handling of the currently active Lua script.
- Lua menu callback errors are now isolated and logged instead of being allowed to destabilize the entire menu.

### Lua Stability Improvements

- Fixed invalid Lua script-index handling.
- Added validation before registering Lua callbacks.
- Improved callback execution safety by avoiding mutation of the same callback container while it is being iterated.
- Added additional checks around Lua state lifetime and callback cleanup.
- Added protection against invalid `pmove` access in LocalPlayer Lua functions.
- Added protection against invalid command pointers and missing engine callbacks.
- Improved Player Lua API bounds checking.
- Added missing entity checks before reading player origin and distance data.
- Added safer PlayerInfo access for player names and ping.
- Improved Lua sound initialization validation.
- Invalid Lua sound handles are no longer inserted into the active sound list.
- Added ImGui frame/window state validation for Lua-rendered UI.
- Improved handling of Lua UI functions called outside a valid ImGui render context.
- Fixed unsafe format-string usage in several Lua-facing text functions.
- Improved Lua notification text safety.
- Fixed Lua dynamic-sound origin conversion so X, Y and Z coordinates are assigned correctly.

### Logging and Runtime Safety

- Increased logging buffers.
- Replaced unsafe formatting paths with bounded formatting where applicable.
- Improved Sakura log output safety.
- Improved Lua error logging.
- Fixed unsafe console formatting behavior.
- Added additional null checks around engine callbacks and runtime pointers.
- Fixed an invalid ImGui window pointer argument used by the fullscreen overlay.

### Notification System 2.0

- Completely redesigned Sakura notifications.
- Added modern card-style toast notifications.
- Added rounded notification backgrounds.
- Added subtle drop shadows.
- Added Sakura accent strip and status marker.
- Added **SAKURA** branding inside notification cards.
- Added notification progress/lifetime bar.
- Added smoother fade-in and fade-out handling.
- Added slide animation while notifications appear and disappear.
- Improved notification stacking and spacing.
- Improved left, center and right positioning behavior.
- Preserved optional **Text only** mode.
- Added **Preview notification** button to the Notifications menu.
- Added safer notification timing using 64-bit tick timing.
- Added safer notification string handling.
- Improved minimum notification duration handling.

### Backtrack / Fake Latency Repair

- Reworked Fake Latency sequence timing to use actual millisecond timing.
- Fixed the old timing mismatch that could prevent configured latency values from being applied correctly.
- Added safer sequence-history handling.
- Added validation for `client_static`, `client_state`, `pmove` and network-related pointers.
- Added safer handling of missing CVar pointers.
- Improved latency clamping.
- Improved interpolation-time handling.
- Reworked historical-position lookup and interpolation.
- Improved selection of older/newer history records.
- Added protection against invalid or empty history entries.
- Added movement-distance validation before accepting reconstructed positions.
- Stopped modifying the server `sv_maxunlag` CVar from the client-side calculation path.
- Improved Netchan pointer handling.
- Improved reconstruction of the rendered historical player position.
- Backtrack visuals now use the calculated historical origin more consistently.

---

## What's New in v2.0.1

Version 2.0.1 focuses primarily on integrating the repaired Backtrack system with Sakura's aiming systems.

### Backtrack-Aware Legit Aimbot

- Legit Aimbot can now evaluate historical Backtrack hitboxes when Backtrack/Fake Latency is enabled.
- Current and historical aim records are tracked separately.
- Historical hitboxes are reconstructed from the player's calculated Backtrack origin.
- FOV calculations are recalculated for the historical hitbox position.
- Backtrack targets use world visibility tracing because the historical model is not a live physics entity at its previous location.
- Legit target drawing now follows the exact current or historical record selected by the aimbot.
- Spread/intersection validation now follows the selected historical record when required.

### Backtrack-Aware Rage Aimbot

- Rage Aimbot can now evaluate historical Backtrack hitboxes when Backtrack/Fake Latency is enabled.
- Backtrack records participate in the existing Rage target-selection modes.
- Rage target state now remembers whether the selected target is a current or historical record.
- Backtrack visibility uses world tracing to avoid relying on the current live player entity.
- Rage wall-penetration calculations remain available for Backtrack candidates.
- Rage target drawing now follows the exact record selected by the aimbot.

### Backtrack Record Handling

- Added a dedicated Backtrack flag to aim records.
- Added reconstructed historical aim records alongside current player records.
- Historical hitbox centers and multi-points are offset to the reconstructed Backtrack origin.
- Historical hitbox FOV values are recalculated after reconstruction.
- Triggerbot ignores duplicated Backtrack aim records to prevent unintended duplicate processing.
- Knifebot ignores duplicated Backtrack aim records.
- Anti-Aim target evaluation ignores duplicated Backtrack records.
- Improved historical player entity state synchronization by applying the reconstructed position consistently to entity origin state fields.

### Fake Latency Follow-up Fix

- Sequence history no longer clears itself when Sakura intentionally rewinds `incoming_sequence` for Fake Latency.
- This prevents the Backtrack/Fake Latency history from continuously resetting during normal use.

### Version and Branding

- Updated Sakura version to **v2.0.1**.
- Updated the version displayed below the Sakura menu title.
- Updated the footer version to **v2.0.1**.
- Added a v2.0.1 version manifest to the refreshed source package.

---

## Lua Scripting

Sakura includes Lua scripting through LuaBridge.

Scripts can interact with supported Sakura/game helpers, register callbacks and render custom ImGui elements.

In v2.x, Lua management is integrated directly into the main Sakura menu:

1. Place your `.lua` scripts inside the `scripts` directory next to `sakura.dll`.
2. Open Sakura and select the **Lua** tab.
3. Use **Reload Lua** after adding or editing scripts.
4. Scripts that register menu-render callbacks will appear in the Lua Workspace.
5. Lua runtime errors are written to `sakura.log` for easier debugging.

The Lua subsystem received extensive safety fixes in v2.0.0, but further API cleanup and expansion is still planned.

---

## How To Build

To build Sakura, you will need:

- [Git](https://git-scm.com/) or a downloaded copy of the repository.
- [Visual Studio](https://visualstudio.microsoft.com/) with C++ development tools installed.
- DirectX SDK for the loader.
- Repository submodules initialized correctly.

Sakura can be compiled with **Visual Studio 2019** or **Visual Studio 2022**.

Clone the refreshed repository with all submodules:

```bash
git clone --recurse-submodules https://github.com/D4NTE98/sakura.git
```

If the repository was already cloned without submodules:

```bash
git submodule sync --recursive
git submodule update --init --recursive
```

Open `sakura.sln` in Visual Studio and build the required configuration.

---

## Known Issues

The following legacy issues are not addressed by the v2.0.0-v2.0.1 refresh and may still be present:

- Perfect Silent FOV behavior may not work correctly.
- `CreateBeamPoint` may not work correctly from Lua scripts.
- The first snapshot may be taken twice when using the anti-screenshot feature.

If you encounter a reproducible crash or feature regression, include the relevant configuration, Lua script if applicable, and `sakura.log` when reporting it.

---

## Roadmap

- Continue improving and documenting the Lua API.
- Add cleaner Save/Load helpers for Lua scripts.
- Continue UI consistency improvements.
- Continue Backtrack and aiming-system validation.
- Resolve remaining legacy bugs.
- Improve runtime diagnostics and error reporting.

---

## Credits

### v2 Refresh

- [D4NTE](https://github.com/D4NTE98) — v2.x refresh, UI modernization, Lua stability improvements, Backtrack repairs and v2.0.1 aiming integration.

### Original Project / Dependencies

- [nc-gp / Sakura](https://github.com/nc-gp/sakura) — original Sakura project.
- [BloodSharp](https://github.com/BloodSharp)
- [Dear ImGui](https://github.com/ocornut/imgui)
- [Lua](https://www.lua.org/)
- [LuaBridge](https://github.com/vinniefalco/LuaBridge)
- [Microsoft Detours](https://github.com/microsoft/Detours)

Special thanks to everyone who contributed to the original Sakura codebase and its dependencies.

---

## Contributing

Contributions, fixes and improvements are welcome.

You can help by:

- Opening pull requests with tested fixes or improvements.
- Reporting reproducible bugs.
- Improving Lua functions and documentation.
- Testing v2.x changes on different Counter-Strike 1.6 configurations.

Repository:

https://github.com/D4NTE98/sakura

---

## License

Sakura is licensed under the **MIT License**.

See the repository [LICENSE](https://github.com/D4NTE98/sakura/blob/main/LICENSE) file for more information.
