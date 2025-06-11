# ecs-sim-framework

A simple ECS (Entity-Component-System) simulation framework in C++.

## Structure

As in any ECS framework, there are three main elements:
- **Entity**: A unique object in the simulation
- **Component**: A piece of data that describes an aspect of an entity
- **System**: A callable that operates on entities with specific components

### Entity

An entity can be uniquely identified by an ID. Entity can have arbitrary components attached to it.
When working with entities, you receive an `Entity` handle from the framework which provides some nice utilities.

### Components

Components are structures holding data. They don't contain any logic.
To create a component, you usually just define a simple struct. Aggregate types are ideal for this.
A component instance then can be added to an entity.

### Systems

Systems are callables that handle events in the simulation and operate on entities with specific components.
To create a system, you define a struct with overloaded `operator()` accepting the appropriate event as the first argument.
Systems are instantiated once at the start of the simulation and kept throughout the simulation lifecycle.
They can thus maintain state, even though it is recommended to keep most of the state in components.
Event handlers in systems can optionally receive a `Context` object as the second argument, which provides access to the simulation context and utilities.

### Events

Events are used to trigger systems at specific stages of the simulation.
They are just simple tag types that don't carry any data (for now).
The most useful event is probably `Cycle` which is fired in every simulation cycle.

### Views

The `Context` object received by event handlers in systems provides access to `View`s.
A `View` is a lightweight view over entities that have a specific set of components.
It provides a `for_each` method but also satisfies the range concept, so you can use it in range-based for loops and even in the standard library algorithms.

### Library

The framework is included with a small library ([sim/lib/](include/sim/lib/) in the sim::lib namespace)
of Components and Systems that can be used in a wide range of simulations.
Examples include `Transform` component for position, a `Movement` system and a `Render` system.

## Examples

Check the [examples/](examples/) directory for some example simulations.
The basic usage is fully shown in [examples/Dummy.cpp](examples/Dummy.cpp).

### Creating a Component

```cpp
struct ExampleComponent {
    int value = 5;
};
```

### Creating a System

```cpp
struct ExampleSystem {
    void operator()(sim::event::Cycle, sim::Context ctx) {
        ctx.view<ExampleComponent>().for_each([](Entity& entity, ExampleComponent& component) {
            component.value += 1;
        });
    }
};
```

### Creating a Simulation

```cpp
Simulation<ExampleSystem>() s; // Specify the system types to be used

// or using the fluent interface
auto s = Simulation<>()
    .with_systems<ExampleSystem>();
```

### Creating an Entity

```cpp
Entity e = s.create()
    .emplace<ExampleComponent>(1); // Pass constructor arguments to the component
        
ExampleComponent c;
s.create()
    .push_back(c); // Push back an existing component instance
```

### Running the Simulation

```cpp
s.run(100); // Run the simulation for 100 cycles
```

## Acknowledgements

This framework uses [raylib](https://www.raylib.com/) and [raylib-cpp](https://github.com/RobLoach/raylib-cpp) for the included renderer.

## Todo

- way to make an entity type (set of components) easily: create gets the types, then is enforced that all components are emplaced before start 
- pick the smallest storage in view foreach
- storage specialization for empty types
- spatial indexing
- foreach function arg through concepts
- auto add/ensure Target when a Target provider is added
