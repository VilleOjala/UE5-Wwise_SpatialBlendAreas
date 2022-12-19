# UE5-Wwise_SpatialBlendAreas

# Purpose

An Unreal Engine 5 plugin for implementing (nested) polygon-shaped blend areas and, for a given world position, 
calculating interdependent area weights based on user-defined spatial blend settings and order of priority. 

This base functionality can be used in conjunction with Audiokinetic Wwise audio middleware for e.g. implementing seamless 
ambience transitions in an outdoor open world setting with complexly-shaped and intertwining biomes. This is achieved by tying the 
calculated weight data to locally scoped RTPCs: An `AkGameObject` instance can be associated with one or more blend areas and the 
RTPC-value corresponds to the sum of the weights of referenced areas in the current listener location. 

Inside the Wwise authoring application the generated blend weight RTPC data can be used to manipulate whatever is appropriate in a particular implementation scenario. Just to give an example, automating linear volume would be a good candidate in the aforementioned smooth ambience transition use case, but one could also utilize the RTPC data to drive the frequency / probability of any area-specific one-shots (fauna, flora etc.) to achieve overall gradual transformation in soundscape with no hard cut-off or crossfade points.

# Vertical & horizontal blend areas

Blend areas are implemented with two `AActor` –derived classes: `AHorizontalBlendArea` and `AVerticalBlendArea`. For both blend area types, the inside / outside -status of a given measurement position is evaluated on an XY-plane, meaning that neither the height of the user-defined spline points nor the Z-value of the measurement position have an effect on the containment test. Blend area polygons are implemented with Unreal Engine’s `USplineComponent` class. For the containment tests to work correctly, the spline interpolation mode has to be set as “Linear”. Also, the less vertices a spline-defined polygon contains, the more performant the containment tests will be. 

If a measurement position is outside a blend area polygon the weight of that area is always zero. When the position is inside an area the blend weight is determined differently for horizontal and vertical blend area types. 

For horizontal blend areas the weight is calculated as follows: 

1) Find the shortest distance from the measurement position to the edge of the blend area polygon (on an XY-plane).
2) Relate this distance to the user-set `BlendDistance` parameter; when the distance is equal to or greater than the blend distance the weight for that area is 100%.

For vertical blend areas: 

1) Relate the world space height of the measurement position to the area-specific blend start height, defined by the parameter `BlendStartHeight`.
2) If the measurement position height is below or on level with the start height, the current weight of the area is 0%.
3) If the measurement position height is equal to or greater than `BlendStartHeight` + `BlendDistance`, the area weight is 100%.

To continue with the ambience transition example above, you can combine and nest (by utilizing priorities) the two blend area types to create ambient experiences that change smoothly both on vertical and horizontal axes. 

# Blend weight managers

The role of blend weight managers is to distribute the overall available weight budget (i.e. 100%) between multiple blend areas based on their relative priorities and blend weights calculated in isolation.

The weight distribution is done in the following manner:

1) Starting with the highest found priority, process blend areas one priority group at a time. 
2) The isolated weights in a priority group are summed together and then subtracted from the total weight budget (i.e. 100%). If the sum exceeds the remaining total weight budget, the isolated weights are discarded and the remaining budget (if any) is distributed between the members of the priority group based on their relative isolated weights. 

The base class for blend weight managers is `ABlendWeightManager`. To create a custom implementation utilizing the weighting behaviour described above, the manager Actor should be populated with components that inherit from `UActorComponent` and implement the `IBlendWeightInterface` interface. By default, the world position used for weight calculations is the first audio listener position retrieved from the `FAudioDevice`, but this behaviour can overridden with the virtual method `GetBlendPosition()`.

In order to work with the Wwise integration, the derived class `AWwiseBlendWeightManager` should be used and populated with `UWwiseBlendAreaEvent` Actor Component instances. `UWwiseBlendAreaEvent` inherits from `UAkComponent`, which is a part of the Audiokinetic Wwise’s Unreal Engine integration and couples one or more blend areas with a `UAkAudioEvent` instance. In order to correctly communicate the weight data to the audio engine, each component instance should be assigned with an RTPC that has a range from 0 to 100, with the default value of 0. By default, the measurement position for weight calculations is the position of the Wwise audio listener (either the default listener or the spatial audio listener). The system assumes that only one audio listener is being used; if a more complicated implementation is required, again override the `GetBlendPosition()` –method.

If the Wwise room-portal spatial audio features are being used, it is possible to have the `AWwiseBlendWeightManager` to implement global states for inside vs. outside room situations. These states may be useful for e.g. overriding the blend area -based ambience approach whenever the listener is inside any spatial audio room and using the Room Tones instead. In the manager, assign the default ‘None’ state to `NoneState` and the user-created state for being inside a spatial audio room to `InsideRoomState`. 

# Workflow hints

To make the drawing of polygon areas much more efficient, add a keyboard shortcut for duplicating the currently selected spline point (_Editor Preferences -> Keyboard Shortcuts -> Spline Component Visualizer -> Duplicate Spline Point_). 

# Dependencies

The plugin was developed on Unreal Engine version 5.0.3 and Wwise integration version 2022.1.0.8070.2495. 

The plugin consists of two modules: **SpatialBlendAreas** for the core functionality and **WwiseIntegration** for the Wwise integration features. The core module has no dependencies to Wwise, so if only the core blend area functionality is needed the **WwiseIntegration** module can deleted, alongside with removing the Wwise -related plugin and module references from the file _SpatialBlendAreas.uplugin_.  
