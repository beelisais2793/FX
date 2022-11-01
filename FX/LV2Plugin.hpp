#include "lilv/lilv.h"
#include "lilv/lilvmm.hpp"
#include "lv2/urid/urid.h"
#include "lv2/core/lv2.h"
#include "lv2/ui/ui.h"
#include "uri_table.h"
#include <iostream>
#include <map>
#include <string>
using namespace std;


// a loaded lv2 plugin
struct LV2Plugin : public MonoFXProcessor
{
    size_t controls,audios,ins,outs,total;
    size_t input_port,output_port;
    /*
    size_t input_port_L = 0;
    size_t input_port_R = 0;
    size_t output_port_L = 0;
    size_t output_port_R = 0;
    */
    Lilv::Plugin   plugin;
    Lilv::Instance * instance;
    Lilv::World *world;
    std::vector<std::vector<float>> connections;    
    std::vector<float> port_min, port_max, port_default;    
    std::vector<std::string>  port_names;
    
    LV2Plugin() : MonoFXProcessor(), plugin(NULL) {
        instance = NULL;
    }
    ~LV2Plugin() {
        if(instance) delete instance;
    }
    void setControl(int port, float value) {
        connections[port] = value;
    }
    float& operator[](size_t index) {
        return connections[index][0];
    }
    std::string lookup(size_t index) {
        return port_names[index];
    }
    int lookup(const std::string& name) {
        for(size_t i = 0; i < port_names.size(); i++)
            if(name == port_names[i]) return i;
        return -1;
    }
    void ProcessBlock(size_t n, float * in, float * out)
    {        
        memcpy(connections[input_port].data(),in,n*sizeof(float));
        instance->run(n);
        memcpy(out,connections[output_port].data(),n*sizeof(float));
    } 
    /*   
    void Run(size_t n, float ** in, float ** out)
    {        
        memcpy(connections[input_port_L].data(),in[0],n*sizeof(float));
        memcpy(connections[input_port_R].data(),in[1],n*sizeof(float));
        instance->run(n);
        memcpy(out[0],connections[output_port_L].data(),n*sizeof(float));
        memcpy(out[1],connections[output_port_R].data(),n*sizeof(float));
    } 
    */   
    void Inplace(size_t n, float * buffer)
    {
        Run(n,buffer,buffer);
    }
    void Randomize() {
        for(size_t i = 0; i < connections.size(); i++)
        {
            if(i == input_port || i == output_port) continue;
            float r = port_min[i] + (port_max[i]-port_min[i])*noise.rand();
            connectios[i][0] = r;
        }
    }
};


struct LV2Plugins
{
    map<std::string,std::string> plugin_map;
    std::vector<std::string> plugin_names;
    Lilv::World world;            
    Lilv::Plugins plugins;    
    URITable uri_table;
  
    LV2Plugins() : plugins(NULL)
    {        
        world.load_all();
        plugins = (Lilv::Plugins)world.get_all_plugins();        
        std::cout << plugins.size() << std::endl;
        auto iter = plugins.begin();
        auto plugin = (Lilv::Plugin)plugins.get(iter);                
        std::cout << plugin.get_uri().as_string() << std::endl;
        
        while(!plugins.is_end(iter)) {    
            auto plugin = plugins.get(iter);
            string name = plugin.get_name().as_string();
            string uri  = plugin.get_uri().as_string();
            plugin_map[name] = uri;
            plugin_names.push_back(name);
            iter = plugins.next(iter);
        }        
        uri_table_init(&uri_table);
    }
    ~LV2Plugins() {

    }
    std::string GetURI(std::string s) {
        return plugin_map[s];
    }
    void listPlugins( )
    {
        Lilv::Plugins* pluginsList = &plugins;
        // loop over the array of plugins, printing data for each.
        for ( LilvIter* i = pluginsList->begin(); !pluginsList->is_end(i); i = pluginsList->next(i) )
        {
            // Get a variable that denotes a plugin in the library
            Lilv::Plugin p = pluginsList->get(i);
            
            // Nodes represent information, this one the plugins name
            Lilv::Node nameNode = p.get_name();
            std::cout << nameNode.as_string() << std::endl;
            
            // this one the plugins uri
            Lilv::Node uriNode = p.get_uri();
            std::cout << uriNode.as_uri() << std::endl << std::endl;
        }
    }
    void printNode(Lilv::Node* tmp)
    {
        // helper function to print a node's contents
        if ( tmp->is_string() )
            std::cout << "String: " << tmp->as_string() << std::endl;
        
        if ( tmp->is_uri() )
            std::cout << "URI: " << tmp->as_uri() << std::endl;
        
        if ( tmp->is_float() )
            std::cout << "float: " << tmp->as_float() << std::endl;
        
        if ( tmp->is_int() )
            std::cout << "int: " << tmp->as_int() << std::endl;
    }


    void printPlugin(Lilv::Plugin* plugin)
    {
        std::cout << "Plugin info: " << std::endl;
        std::cout << "Name: " << plugin->get_name().as_string() << std::endl;
        std::cout << "URI: " << plugin->get_uri().as_string() << std::endl;
        
        if ( plugin->get_author_name().is_string() )
            std::cout << "Plugin by " << plugin->get_author_name().as_string()    << ""    << std::endl;
        
        if ( plugin->get_author_name().is_string() )
            std::cout << "Email     " << plugin->get_author_email().as_string()    << ""    << std::endl;
        
        if ( plugin->get_author_name().is_string() )
            std::cout << "Homepage  " << plugin->get_author_homepage().as_string()  << ""    << std::endl;
        
        std::cout << "Plugin has " << plugin->get_num_ports()          << " ports"  << std::endl;
        
        std::cout << std::endl;
    }
    void infoPlugin(Lilv::Plugin* plugin)
    {
        // print out the info we have on this plugin, author, website etc
        printPlugin (plugin);
        
        // get features that this plugin supports, and print them
        Lilv::Nodes supportedFeatures = plugin->get_supported_features();
        
        std::cout << "Supported features: \n";
        for ( LilvIter* i = supportedFeatures.begin(); !supportedFeatures.is_end(i); i = supportedFeatures.next(i) )
        {
            Lilv::Node tmp = supportedFeatures.get(i);
            printNode(&tmp);
        }
        std::cout << std::endl;
        
        // get features that this plugin *requires* and print them
        Lilv::Nodes requiredFeatures = plugin->get_required_features();
        std::cout << "Required features: \n";
        for ( LilvIter* i = requiredFeatures.begin(); !requiredFeatures.is_end(i); i = requiredFeatures.next(i) )
        {
            Lilv::Node tmp = requiredFeatures.get(i);
            printNode (&tmp);
            
        }
        std::cout << std::endl;
        
    }
    LV2Plugin* LoadPlugin(const std::string& uri)
    {
        LV2Plugin *lv2 = new LV2Plugin();

        //string uri = plugin_map[name];
        //cout << uri << endl;
        auto plugin_uri = world.new_uri(uri.c_str());        
        lv2->plugin = plugins.get_by_uri(plugin_uri);        
        lv2->world  = &world;

        Lilv::Node audio_class = world.new_uri("http://lv2plug.in/ns/lv2core#AudioPort");
        Lilv::Node control_class = world.new_uri("http://lv2plug.in/ns/lv2core#ControlPort");
        Lilv::Node in_class = world.new_uri("http://lv2plug.in/ns/lv2core#InputPort");
        Lilv::Node out_class = world.new_uri("http://lv2plug.in/ns/lv2core#OutputPort");
                    
        lv2->controls = lv2->plugin.get_num_ports_of_class(control_class, NULL);
        std::cout << lv2->controls << std::endl;
        lv2->audios = lv2->plugin.get_num_ports_of_class(audio_class, NULL);
        std::cout << lv2->audios << std::endl;
        lv2->ins = lv2->plugin.get_num_ports_of_class(in_class, NULL);
        std::cout << lv2->ins << std::endl;
        lv2->outs = lv2->plugin.get_num_ports_of_class(out_class, NULL);
        std::cout << lv2->outs << std::endl;
        lv2->total = lv2->plugin.get_num_ports();
        lv2->port_min.resize(lv2->total);
        lv2->port_max.resize(lv2->total);
        lv2->port_default.resize(lv2->total);
        lv2->port_names.resize(lv2->total);
        lv2->plugin.get_port_ranges_float(lv2->port_min.data(),lv2->port_max.data(),lv2->port_default.data());

        for(size_t i = 0; i < lv2->total; i++)
        {
            Lilv::Port port =  lv2->plugin.get_port_by_index(i);
            std::cout << "Port#" << i << "," << Lilv::Node(port.get_name()).as_string() << std::endl;            
            lv2->port_names[i] = Lilv::Node(port.get_name()).as_string();            
            //if(!strcmp(port.get_symbol().as_string(),"in_l") lv2->input_port_L = i; 
            //else if(!strcmp(port.get_symbol().as_string(),"in_R") lv2->input_port_R = i; 
            //else if(!strcmp(port.get_symbol().as_string(),"out_l") lv2->output_port_L = i; 
            //else if(!strcmp(port.get_symbol().as_string(),"out_R") lv2->output_port_R = i; 
            if(port.is_a(in_class))  lv2->input_port = i;
            else if(port.is_a(out_class)) lv2->output_port = i;
        }       
        
        Lilv::Nodes nodes = lv2->plugin.get_required_features();
        LilvIter * iter = nodes.begin();
        while(!nodes.is_end(iter))
        {
            Lilv::Node n = nodes.get(iter);
            std::cout << n.as_string() << std::endl;
            iter = nodes.next(iter);
        }
        LV2_URID_Map       map           = {&uri_table, uri_table_map};
        LV2_Feature        map_feature   = {LV2_URID_MAP_URI, &map};
        LV2_URID_Unmap     unmap         = {&uri_table, uri_table_unmap};        
        LV2_Feature        unmap_feature = {LV2_URID_UNMAP_URI, &unmap};
        const LV2_Feature* features[]    = {&map_feature, &unmap_feature, NULL};    
               
        lv2->instance= Lilv::Instance::create(lv2->plugin,44100.0f,(LV2_Feature * const *)features);
        lv2->instance->activate();
        lv2->connections.resize(lv2->total);
        for(size_t i = 0; i < lv2->total; i++)
        {
            lv2->connections[i].resize(1024);
            lv2->instance->connect_port(i,&lv2->connections[i][0]);
        }
        return lv2;        
    }        
    
};

