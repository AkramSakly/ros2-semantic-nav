# One object that the robot has seen and placed on the map.
uint32 id                      # unique id given by the mapper
string label                   # COCO class name, e.g. "chair"
geometry_msgs/Point position   # position in the map frame (metres)
float32 confidence             # mean detector confidence over all observations
uint32 observations            # how many times this object was seen
builtin_interfaces/Time last_seen
