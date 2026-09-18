# Known inconsistencies

Reviewed on 2026-09-18. These notes record differences found by reading the repository; no runtime or hardware validation was performed. Existing code and instructions are preserved.

## ArduPilot simulator versus PX4 controller branches

[The simulator launch](../src/usv_simulator/launch/usv_simulator.launch.py) includes the ArduPilot SITL DDS setup. The currently reviewed default branches of [usv_controller](https://github.com/Sub-Horizon-NTNU/usv_controller) and [transform_broadcaster](https://github.com/Sub-Horizon-NTNU/transform_broadcaster) use PX4 `/fmu/*` interfaces.

Compatibility must be established through matching versions or an explicit adapter. The existence of `simulator_mode` elsewhere does not itself establish compatibility.

## README images are missing

The original README references `readme_images/sim_photo_usv.png` and scenario illustrations under `readme_images/`. That directory is absent in the reviewed default-branch tree, so those local image links cannot render from the repository.

No simulator code, dependencies or original instructions were modified by this documentation review.
