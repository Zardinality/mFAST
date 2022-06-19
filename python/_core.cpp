#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <mfast.h>
#include <mfast/coder/fast_decoder.h>
#include <mfast/json/json.h>
#include <mfast/xml_parser/dynamic_templates_description.h>

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)


namespace py = pybind11;

using std::string;
using std::ostringstream;
using mfast::templates_description;
using mfast::dynamic_templates_description;
using mfast::fast_decoder;
using mfast::message_cref;

struct MsgPack{
    uint32_t template_id;
    string template_name;
    string msg_json;
};

class Decoder{
    fast_decoder m_decoder;
public:
    Decoder(const string& template_str){
        add_template(template_str);
    }
    void add_template(const string& template_str){
        dynamic_templates_description dtd{template_str};
        const templates_description* descriptions[] = {&dtd};
        m_decoder.include(descriptions);
    }
    std::pair<std::vector<MsgPack>, uint64_t> decode(const string& msg){
        auto start = msg.c_str();
        auto end = start + msg.size();
        std::vector<MsgPack> res;
        while(start<end){
            message_cref decoded_msg = m_decoder.decode(start, end);
            ostringstream json_message;
            try
            {
                mfast::json::encode(json_message, decoded_msg, 0);
            }
            catch(const std::exception& e)
            {
                std::cerr << e.what() << '\n';
                // throw(e);
                break;
            }
            
            res.push_back({decoded_msg.id(), decoded_msg.name(), json_message.str()});
        }
        return {res, start-end};
    }
};


PYBIND11_MODULE(_core, m) {
    m.doc() = R"pbdoc(
        Pybind11 example plugin
        -----------------------
        .. currentmodule:: scikit_build_example
        .. autosummary::
           :toctree: _generate
           add
           subtract
    )pbdoc";
    py::class_<MsgPack>(m, "MsgPack")
        .def(py::init<uint32_t, const std::string &, const std::string &>())
        .def_readonly("id", &MsgPack::template_id)
        .def_readonly("name", &MsgPack::template_name)
        .def_readonly("json", &MsgPack::msg_json);
    py::class_<Decoder>(m, "Decoder")
        .def(py::init<const std::string &>())
        .def("add_template", &Decoder::add_template)
        .def("decode", &Decoder::decode);


#ifdef VERSION_INFO
    m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
    m.attr("__version__") = "dev";
#endif
}