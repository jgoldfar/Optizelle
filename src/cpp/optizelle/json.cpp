#include "optizelle/json.h"

namespace Optizelle {
    namespace json {
        // Parses a JSON file and returns the root
        Json::Value parse(
            std::string const & fname
        ) {
            // Read in the input file
            auto root = Json::Value();
            auto builder = Json::CharReaderBuilder();
            auto reader = std::unique_ptr<Json::CharReader> (
                builder.newCharReader());
            auto file = std::ifstream(fname.c_str(),std::ifstream::in);
            auto err = std::string();
            bool parsingSuccessful = Json::parseFromStream(
                builder,
                file,
                &root,
                &err);
            if(!parsingSuccessful)
                throw Exception::t(__LOC__
                    + ", failed to parse the optimization parameter "
                    "file:  " + err);

            // Return the root
            return root;
        }

        // Writes a JSON spec to file
        void write_to_file(
            std::string const & fname,
            Json::Value const & root
        ) {
            // Open a file for writing
            std::ofstream fout(fname.c_str());
            if(!fout.is_open())
                throw Exception::t(__LOC__
                    + ", while writing the restart file, unable to open "
                    "the file: " + fname + ".");

            // Write out the json tree
            fout << root;
            if(fout.bad())
                throw Exception::t(__LOC__
                    + ", while writing the restart file, unable to write "
                    "the json tree");
        }

        // Safely reads from a json tree
        namespace read {
            // Read a natural
            Natural natural(
                Json::Value const & json,
                std::string const & name
            ) {
                // Set the error message
                std::string const err_msg = "Invalid JSON parameter: "
                    + name + " contains an invalid natural.";

                // As long as we have an unsigned integer, grab it
                if(json.isUInt() || json.isUInt64())
                    return Natural(Json::Value::UInt64(json.asUInt64()));

                // If we have an integer, grab it if it's positive
                else if(json.isInt() || json.isInt64()) {
                    Integer val(json.asInt64());
                    if(val>=0)
                        return Natural(val);
                    else
                        throw Exception::t(__LOC__ + ", " + err_msg);

                // Anything else is an error
                } else
                    throw Exception::t(__LOC__ + ", " + err_msg);
            }

            // Read a string
            std::string string(
                Json::Value const & json,
                std::string const & name
            ) {
                // Set the error message
                std::string const err_msg = "Invalid JSON parameter: "
                    + name + " is not a valid parameter.";

                // As long as we have an unsigned integer, grab it
                if(json.isString())
                    return json.asString();

                // Anything else is an error
                else
                    throw Exception::t(__LOC__ + ", " + err_msg);
            }
        }

        // Writes into a json tree
        namespace write {
            // Write a natural
            Json::Value natural(Natural const & val) {
                return Json::Value::UInt64(val);
            }
        }

        // Routines to serialize lists of elements for restarting
        namespace Serialize{

            // Naturals
            void naturals(
                typename RestartPackage <Natural>::t const & nats,
                std::string const & vs,
                Json::Value & root
            ) {
                // Create some type shortcuts
                typedef typename RestartPackage <Natural>::t Naturals;

                // Loop over all the naturals and serialize things
                for(typename Naturals::const_iterator item = nats.cbegin();
                    item!=nats.cend();
                    item++
                )
                    root[vs][item->first]=write::natural(item->second);
            }

            // Parameters
            void parameters(
                typename RestartPackage <std::string>::t const & params,
                std::string const & vs,
                Json::Value & root
            ) {
                // Create some type shortcuts
                typedef typename RestartPackage <std::string>::t Params;

                // Loop over all the params and serialize things
                for(typename Params::const_iterator item = params.cbegin();
                    item!=params.cend();
                    item++
                )
                    root[vs][item->first]=item->second;
            }
        }

        // Routines to deserialize lists of elements for restarting
        namespace Deserialize{

            // Naturals
            void naturals(
                Json::Value const & root,
                std::string const & vs,
                typename RestartPackage <Natural>::t & nats
            ) {
                // Loop over all the names in the root
                for(Json::ValueConstIterator itr=root[vs].begin();
                    itr!=root[vs].end();
                    itr++
                ){
                    // Grab the natural
                    std::string name(itr.key().asString());
                    nats.emplace_back(name,std::move(
                        read::natural(root[vs][name],name)));
                }
            }

            // Parameters
            void parameters(
                Json::Value const & root,
                std::string const & vs,
                typename RestartPackage <std::string>::t & params
            ) {
                // Loop over all the names in the root
                for(Json::ValueConstIterator itr=root[vs].begin();
                    itr!=root[vs].end();
                    itr++
                ){
                    // Grab the parameter
                    std::string name(itr.key().asString());
                    params.emplace_back(name,std::move(
                        read::string(root[vs][name],name)));
                }
            }
        }
    }
}
