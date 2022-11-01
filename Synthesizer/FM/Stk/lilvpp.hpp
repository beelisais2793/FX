
namespace Lilv {

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"


struct Instance;
struct Node;
struct Nodes;
struct Plugin;
struct PluginClass;
struct PluginClasses;
struct Plugins;
struct Port;
struct ScalePoint;
struct ScalePoints;
struct UI;
struct UIs;
struct World;

LILV_DEPRECATED
static inline const char*
uri_to_path(const char* uri)
{
  return lilv_uri_to_path(uri);
}




#pragma GCC diagnostic pop
# 70 "temp.h"
struct Node {
  inline Node(const LilvNode* node)
    : me(lilv_node_duplicate(node))
  {}

  inline Node(const Node& copy)
    : me(lilv_node_duplicate(copy.me))
  {}

  inline Node& operator=(const Node& rhs)
  {
    if (&rhs != this) {
      lilv_node_free(me);
      me = lilv_node_duplicate(rhs.me);
    }
    return *this;
  }

  inline Node(Node&& other) noexcept
    : me(other.me)
  {
    other.me = nullptr;
  }

  inline Node& operator=(Node&& rhs) noexcept
  {
    if (&rhs != this) {
      me = rhs.me;
      rhs.me = nullptr;
    }
    return *this;
  }

  inline ~Node() { lilv_node_free(me); }

  inline bool equals(const Node& other) const
  {
    return lilv_node_equals(me, other.me);
  }

  inline bool operator==(const Node& other) const { return equals(other); }

  inline operator LilvNode*() const { return me; };

  inline char* get_turtle_token() { return lilv_node_get_turtle_token(me); };
  inline bool is_uri() { return lilv_node_is_uri(me); };
  inline const char* as_uri() { return lilv_node_as_uri(me); };
  inline bool is_blank() { return lilv_node_is_blank(me); };
  inline const char* as_blank() { return lilv_node_as_blank(me); };
  inline bool is_literal() { return lilv_node_is_literal(me); };
  inline bool is_string() { return lilv_node_is_string(me); };
  inline const char* as_string() { return lilv_node_as_string(me); };
  inline bool is_float() { return lilv_node_is_float(me); };
  inline float as_float() { return lilv_node_as_float(me); };
  inline bool is_int() { return lilv_node_is_int(me); };
  inline int as_int() { return lilv_node_as_int(me); };
  inline bool is_bool() { return lilv_node_is_bool(me); };
  inline bool as_bool() { return lilv_node_as_bool(me); };

  LilvNode* me;
};

struct ScalePoint {
  inline ScalePoint(const LilvScalePoint* c_obj)
    : me(c_obj)
  {}

  inline operator const LilvScalePoint*() const { return me; };

  inline const LilvNode* get_label() { return lilv_scale_point_get_label(me); };
  inline const LilvNode* get_value() { return lilv_scale_point_get_value(me); };

  const LilvScalePoint* me;
};

struct PluginClass {
  inline PluginClass(const LilvPluginClass* c_obj)
    : me(c_obj)
  {}

  inline operator const LilvPluginClass*() const { return me; };

  inline Node get_parent_uri() { return lilv_plugin_class_get_parent_uri(me); };
  inline Node get_uri() { return lilv_plugin_class_get_uri(me); };
  inline Node get_label() { return lilv_plugin_class_get_label(me); };
  inline LilvPluginClasses* get_children() { return lilv_plugin_class_get_children(me); };

  const LilvPluginClass* me;
};
# 172 "temp.h"
struct PluginClasses {
  inline PluginClasses(const LilvPluginClasses* c_obj) : me(c_obj) {} inline operator const LilvPluginClasses*() const { return me; }; inline unsigned size() { return lilv_plugin_classes_size(me); }; inline PluginClass get(LilvIter* i) { return lilv_plugin_classes_get(me, i); }; inline LilvIter* begin() { return lilv_plugin_classes_begin(me); }; inline LilvIter* next(LilvIter* i) { return lilv_plugin_classes_next(me, i); }; inline bool is_end(LilvIter* i) { return lilv_plugin_classes_is_end(me, i); }; const LilvPluginClasses* me;;
  inline PluginClass get_by_uri(const LilvNode* uri) { return lilv_plugin_classes_get_by_uri(me, uri); };
};

struct ScalePoints {
  inline ScalePoints(const LilvScalePoints* c_obj) : me(c_obj) {} inline operator const LilvScalePoints*() const { return me; }; inline unsigned size() { return lilv_scale_points_size(me); }; inline ScalePoint get(LilvIter* i) { return lilv_scale_points_get(me, i); }; inline LilvIter* begin() { return lilv_scale_points_begin(me); }; inline LilvIter* next(LilvIter* i) { return lilv_scale_points_next(me, i); }; inline bool is_end(LilvIter* i) { return lilv_scale_points_is_end(me, i); }; const LilvScalePoints* me;;
};

struct Nodes {
  inline Nodes(const LilvNodes* c_obj) : me(c_obj) {} inline operator const LilvNodes*() const { return me; }; inline unsigned size() { return lilv_nodes_size(me); }; inline Node get(LilvIter* i) { return lilv_nodes_get(me, i); }; inline LilvIter* begin() { return lilv_nodes_begin(me); }; inline LilvIter* next(LilvIter* i) { return lilv_nodes_next(me, i); }; inline bool is_end(LilvIter* i) { return lilv_nodes_is_end(me, i); }; const LilvNodes* me;;
  inline bool contains(const Node& node) { return lilv_nodes_contains(me, node); };
  inline Node get_first() { return lilv_nodes_get_first(me); };
};

struct UI {
  inline UI(const LilvUI* c_obj)
    : me(c_obj)
  {}

  inline operator const LilvUI*() const { return me; };

  inline const LilvNode* get_uri() { return lilv_ui_get_uri(me); };
  inline const LilvNode* get_bundle_uri() { return lilv_ui_get_bundle_uri(me); };
  inline const LilvNode* get_binary_uri() { return lilv_ui_get_binary_uri(me); };
  inline const LilvNodes* get_classes() { return lilv_ui_get_classes(me); };




  inline bool is_a(const LilvNode* class_uri) { return lilv_ui_is_a(me, class_uri); };

  const LilvUI* me;
};

struct UIs {
  inline UIs(const LilvUIs* c_obj) : me(c_obj) {} inline operator const LilvUIs*() const { return me; }; inline unsigned size() { return lilv_uis_size(me); }; inline UI get(LilvIter* i) { return lilv_uis_get(me, i); }; inline LilvIter* begin() { return lilv_uis_begin(me); }; inline LilvIter* next(LilvIter* i) { return lilv_uis_next(me, i); }; inline bool is_end(LilvIter* i) { return lilv_uis_is_end(me, i); }; const LilvUIs* me;;
};

struct Port {
  inline Port(const LilvPlugin* p, const LilvPort* c_obj)
    : parent(p)
    , me(c_obj)
  {}

  inline operator const LilvPort*() const { return me; };







  inline LilvNodes* get_value(LilvNode* predicate) { return lilv_port_get_value(parent, me, predicate); };
  inline LilvNodes* get_properties() { return lilv_port_get_properties(parent, me); }
  inline bool has_property(LilvNode* property_uri) { return lilv_port_has_property(parent, me, property_uri); };
  inline bool supports_event(LilvNode* event_uri) { return lilv_port_supports_event(parent, me, event_uri); };
  inline const LilvNode* get_symbol() { return lilv_port_get_symbol(parent, me); };
  inline LilvNode* get_name() { return lilv_port_get_name(parent, me); };
  inline const LilvNodes* get_classes() { return lilv_port_get_classes(parent, me); };
  inline bool is_a(LilvNode* port_class) { return lilv_port_is_a(parent, me, port_class); };
  inline LilvScalePoints* get_scale_points() { return lilv_port_get_scale_points(parent, me); };



  const LilvPlugin* parent;
  const LilvPort* me;
};

struct Plugin {
  inline Plugin(const LilvPlugin* c_obj)
    : me(c_obj)
  {}

  inline operator const LilvPlugin*() const { return me; };

  inline bool verify() { return lilv_plugin_verify(me); };
  inline Node get_uri() { return lilv_plugin_get_uri(me); };
  inline Node get_bundle_uri() { return lilv_plugin_get_bundle_uri(me); };
  inline Nodes get_data_uris() { return lilv_plugin_get_data_uris(me); };
  inline Node get_library_uri() { return lilv_plugin_get_library_uri(me); };
  inline Node get_name() { return lilv_plugin_get_name(me); };
  inline PluginClass get_class() { return lilv_plugin_get_class(me); };
  inline Nodes get_value(const Node& pred) { return lilv_plugin_get_value(me, pred); };
  inline bool has_feature(const Node& feature_uri) { return lilv_plugin_has_feature(me, feature_uri); };
  inline Nodes get_supported_features() { return lilv_plugin_get_supported_features(me); };
  inline Nodes get_required_features() { return lilv_plugin_get_required_features(me); };
  inline Nodes get_optional_features() { return lilv_plugin_get_optional_features(me); };
  inline unsigned get_num_ports() { return lilv_plugin_get_num_ports(me); };
  inline bool has_latency() { return lilv_plugin_has_latency(me); };
  inline unsigned get_latency_port_index() { return lilv_plugin_get_latency_port_index(me); };
  inline Node get_author_name() { return lilv_plugin_get_author_name(me); };
  inline Node get_author_email() { return lilv_plugin_get_author_email(me); };
  inline Node get_author_homepage() { return lilv_plugin_get_author_homepage(me); };
  inline bool is_replaced() { return lilv_plugin_is_replaced(me); };
  inline Nodes get_extension_data() { return lilv_plugin_get_extension_data(me); };
  inline UIs get_uis() { return lilv_plugin_get_uis(me); };
  inline Nodes get_related(const Node& type) { return lilv_plugin_get_related(me, type); };

  inline Port get_port_by_index(unsigned index) const
  {
    return Port(me, lilv_plugin_get_port_by_index(me, index));
  }

  inline Port get_port_by_symbol(LilvNode* symbol) const
  {
    return Port(me, lilv_plugin_get_port_by_symbol(me, symbol));
  }

  inline void get_port_ranges_float(float* min_values,
                                    float* max_values,
                                    float* def_values) const
  {
    return lilv_plugin_get_port_ranges_float(
      me, min_values, max_values, def_values);
  }

  inline unsigned get_num_ports_of_class(LilvNode* class_1, ...) const
  {
    va_list args;
    va_start(args, class_1);

    const uint32_t count =
      lilv_plugin_get_num_ports_of_class_va(me, class_1, args);

    va_end(args);
    return count;
  }

  const LilvPlugin* me;
};

struct Plugins {
  inline Plugins(const LilvPlugins* c_obj) : me(c_obj) {} inline operator const LilvPlugins*() const { return me; }; 
  inline unsigned size() { return lilv_plugins_size(plme); }; 
  inline Plugin get(LilvIter* i) { return lilv_plugins_get(me, i); }; 
  inline LilvIter* begin() { return lilv_plugins_begin(me); }; 
  inline LilvIter* next(LilvIter* i) { return lilv_plugins_next(me, i); }; 
  inline bool is_end(LilvIter* i) { return lilv_plugins_is_end(me, i); }; const LilvPlugins* me;;
  inline Plugin get_by_uri(const LilvNode* uri) { return lilv_plugins_get_by_uri(me, uri); };
};

struct Instance {
  inline Instance(LilvInstance* instance)
    : me(instance)
  {}

  LILV_DEPRECATED
  inline Instance(Plugin plugin, double sample_rate)
  {
    me = lilv_plugin_instantiate(plugin, sample_rate, nullptr);
  }

  LILV_DEPRECATED inline Instance(Plugin plugin,
                                  double sample_rate,
                                  LV2_Feature* const* features)
  {
    me = lilv_plugin_instantiate(plugin, sample_rate, features);
  }

  static inline Instance* create(Plugin plugin,
                                 double sample_rate,
                                 LV2_Feature* const* features)
  {
    LilvInstance* me = lilv_plugin_instantiate(plugin, sample_rate, features);

    return me ? new Instance(me) : nullptr;
  }

  inline operator LilvInstance*() const { return me; };

  inline void connect_port(unsigned port_index, void* data_location) { lilv_instance_connect_port(me, port_index, data_location); }




                                ;

  inline void activate() { lilv_instance_activate(me); };
  inline void run(unsigned sample_count) { lilv_instance_run(me, sample_count); };
  inline void deactivate() { lilv_instance_deactivate(me); };

  inline const void* get_extension_data(const char* uri) const
  {
    return lilv_instance_get_extension_data(me, uri);
  }

  inline const LV2_Descriptor* get_descriptor() const
  {
    return lilv_instance_get_descriptor(me);
  }

  inline LV2_Handle get_handle() const { return lilv_instance_get_handle(me); }

  LilvInstance* me;
};

struct World {
  inline World()
    : me(lilv_world_new())
  {}

  inline ~World() { lilv_world_free(me); }

  World(const World&) = delete;
  World& operator=(const World&) = delete;

  World(World&&) = delete;
  World& operator=(World&&) = delete;

  inline LilvNode* new_uri(const char* uri) const
  {
    return lilv_new_uri(me, uri);
  }

  inline LilvNode* new_string(const char* str) const
  {
    return lilv_new_string(me, str);
  }

  inline LilvNode* new_int(int val) const { return lilv_new_int(me, val); }

  inline LilvNode* new_float(float val) const
  {
    return lilv_new_float(me, val);
  }

  inline LilvNode* new_bool(bool val) const { return lilv_new_bool(me, val); }

  inline Nodes find_nodes(const LilvNode* subject,
                          const LilvNode* predicate,
                          const LilvNode* object) const
  {
    return lilv_world_find_nodes(me, subject, predicate, object);
  }

  inline void set_option(const char* uri, LilvNode* value) { lilv_world_set_option(me, uri, value); };
  inline void load_all() { lilv_world_load_all(me); };
  inline void load_bundle(LilvNode* bundle_uri) { lilv_world_load_bundle(me, bundle_uri); };
  inline const LilvPluginClass* get_plugin_class() { return lilv_world_get_plugin_class(me); };
  inline const LilvPluginClasses* get_plugin_classes() { return lilv_world_get_plugin_classes(me); };
  inline Plugins get_all_plugins() { return lilv_world_get_all_plugins(me); };
  inline int load_resource(const LilvNode* resource) { return lilv_world_load_resource(me, resource); };

  LilvWorld* me;
};

}
