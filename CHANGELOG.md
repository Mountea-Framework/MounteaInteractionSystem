# Changelog

All notable changes to this project will be documented in this file.

## Version 3.1 ##
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
