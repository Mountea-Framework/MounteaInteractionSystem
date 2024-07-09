Current Version: 4.0.0.52

# Changelog

All notable changes to this project will be documented in this file.

** Version 4.0 **
### Changed
- Interfaces are now much more exposed to blueprints
- - almost all functions are now Blueprint Native Events, therefore allows BP overriding
- **Replication** of the whole system
- Example UI is now unified and contains even more examples
- System is updated to work with *Enhanced Input*
- Interactable is now completely passive
- - all logic is driven by Interactor component
- Added new Editor/Debug Build logging to Console and Screen
- Whole interactor system now contains additional safety trace to avoid starting interaction while obstacles are in a way
- - If you have interactable behind a wall and its interaction collision is peaking through it a safety check is cast based on different collision channel (camera by default) to ensure you can see the interactable
- Added regex replacement for better managing Interaction Data in data table
- Added Gameplay Tag support to further refine searching for interactables
- Added automatic UI switching based on Input Mode, Platform & Input Device
- - This way button "A" can be mapped for Xbox Controller and "X" for PS5 Controller (only for Platforms now, hardware ID is not available for now)
- Added dependency on `CommonUI`
- Added dependency on `EnhancedInput`

### Fixed
- Fixed old interaction widgets
- Fixed many small issues
- Fixed loosing interactor after pausing interaction

** Version 3.1 **
### Added 
> - Added: Improved performance for Consoles and Mobile devices
### Fixed
> - Fixed: Wrong state machine decisions when going from Cooldown to Awake for Interactables
### Changed
> - Changed: Deprecated *SnoozeInteractable* functions
> - Changed: Depreacted *Asleep* Interactable State

## Version 3.0 ##
### Added 
> - Added: New Interactor Component Base Class implementing IActorInteractorInterface
> - Added: Child Classes for this new Interactor Base class, replacing existing monolithic ActorInteractorComponent solution
> - Added: New Interactable Base Component class implementing IActorInteractableInterface
> - Added: New child Classes for this new Interactable Base class, replacing existing monolithic ActorInteractableComponent solution
> - Added: New Interactable Widget Interface for easier communication between Widgets and Interactables
> - Added: Editor billboard so Interactable Components are now easier to spot
> - Added: New Overlay Materials (for 5.1 only)
### Fixed
> - Fixed: Missing descriptions for multiple functions
> - Fixed: Missing descriptions for multiple variables
### Changed
> - Changed: Added explicit warning about using old Interactor and Interactable Components
