# Project description

## Structure

An ecosystem consists of **Entities** and an **Environment** and it is run by providing a start **State**.

### Entity

Entity represents a living part of the ecosystem.

Every entity has:
- Type
- Position (2D coordinates)
- Properties
- Behaviour

Every entity can be:
- created
- destroyed

#### Properties

Properties are extra data attached to an Entity. It represents the state of the
individual instance.

#### Behaviour

Behaviour is a set of Actions taken by the Entity in key steps of the simulation.
Each action is associated to (one or multiple) Events when it executes.
It is thus a mapping Event -> Action.

##### Event

Events are fired in key steps of the simulation Cycle.

They are:
- create - fired after creating the Entity
- destroy - fired before destroying the Entity
- cycle - fired at the start of each Cycle
- ~~start - fired once at the start of the first Cycle of the simulation~~
- ~~end - fired at the end of the last Cycle of the simulation~~

##### Action

Action is an atomic functionality of an Entity.
It is represented by a functor.

### Environment

Environment comprises the space for the simulation to play out in.

Environment has:
- dimensions
- Elements

#### Element

Element is a non-active part of the environment.

### State

State is the full configuration of the simulation in a single Cycle.
You need a starting State from which the simulation starts. When the simulation ends, the state at the final
cycle is stored. The simulation progression is stored as a sequence of States.

A State consists of:
- the data of each individual Entity
- the data of each individual Element
- the Cycle number

## Simulation

Simulation consists of discrete Cycles representing the smallest steps of time.

### Cycle

A step of the simulation when all Entities and Elements are processed by firing Events.


# Todo

- foreach views
- renderer separate?
- raylib global namespace problems
- way to make an entity type (set of components) easily: create gets the types, then is enforced that all components are emplaced before start 
- pick smallest storage in view foreach
- storage specialization for empty types
- spatial indexing
- unpack on entity that unpacks the components
- view constness
