/*!
 * @project        The CERN Tape Archive (CTA)
 * @brief          Definitions for parsing the options of the CTA Admin command-line tool
 * @description    CTA Admin command using Google Protocol Buffers and XRootD SSI transport
 * @copyright      Copyright 2017 CERN
 * @license        This program is free software: you can redistribute it and/or modify
 *                 it under the terms of the GNU General Public License as published by
 *                 the Free Software Foundation, either version 3 of the License, or
 *                 (at your option) any later version.
 *
 *                 This program is distributed in the hope that it will be useful,
 *                 but WITHOUT ANY WARRANTY; without even the implied warranty of
 *                 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *                 GNU General Public License for more details.
 *
 *                 You should have received a copy of the GNU General Public License
 *                 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <map>
#include <set>
#include <string>

#include "CtaFrontendApi.hpp"

namespace cta {
namespace admin {

/*!
 * Command line option class
 */
class Option
{
public:
   enum option_t { OPT_CMD, OPT_FLAG, OPT_BOOL, OPT_UINT, OPT_STR, OPT_STR_LIST };

   /*!
    * Constructor
    */
   Option(option_t type, const std::string &long_opt, const std::string &short_opt,
          const std::string &help_txt, const std::string &alias = "") :
      m_type(type),
      m_long_opt(long_opt),
      m_short_opt(short_opt),
      m_help_txt(help_txt),
      m_is_optional(false) {
        m_lookup_key = (alias.size() == 0) ? long_opt : alias;
   }

   /*!
    * Copy-construct an optional version of this option
    */
   Option optional() const {
      Option option(*this);
      option.m_is_optional = true;
      return option;
   }

   /*!
    * Check if the supplied key matches the option
    */
   bool operator==(const std::string &option) const {
      return option == m_short_opt || option == m_long_opt;
   }

   /*!
    * Return the type of this option
    */
   option_t get_type() const { return m_type; }

   /*!
    * Return the number of parameters expected after this option
    */
   int num_params() const { return m_type == OPT_CMD || m_type == OPT_FLAG ? 0 : 1; }

   /*!
    * Return the key for this option
    */
   const std::string &get_key() const { return m_lookup_key; }

   /*!
    * Return whether the option is optional
    */
   bool is_optional() const { return m_is_optional; }

   /*!
    * Validate the command protocol buffer against this option
    *
    * If the option is compulsory and is not present, throws an exception
    */
   void validateCmd(const cta::admin::AdminCmd &admincmd) const;

   /*!
    * Return per-option help string
    */
   std::string help() const {
      std::string help = m_is_optional ? " [" : " ";
      help += (m_type == OPT_CMD) ? "" : m_long_opt + '/' + m_short_opt;
      help += m_help_txt;
      help += m_is_optional ? "]" : "";
      return help;
   }

private:
   option_t    m_type;          //!< Option type
   std::string m_lookup_key;    //!< Key to map option string to Protocol Buffer enum
   std::string m_long_opt;      //!< Long command option
   std::string m_short_opt;     //!< Short command option
   std::string m_help_txt;      //!< Option help text
   bool        m_is_optional;   //!< Option is optional or compulsory
};



/*
 * Type aliases
 */
using cmdLookup_t    = std::map<std::string, AdminCmd::Cmd>;
using subcmdLookup_t = std::map<std::string, AdminCmd::SubCmd>;
using cmd_key_t      = std::pair<AdminCmd::Cmd, AdminCmd::SubCmd>;
using cmd_val_t      = std::vector<Option>;



/*!
 * Command/subcommand help class
 */
class CmdHelp
{
public:
   /*!
    * Constructor
    */
   CmdHelp(const std::string &cmd_long, const std::string &cmd_short,
           const std::vector<std::string> &sub_cmd, const std::string &help_txt = "") :
      m_cmd_long(cmd_long),
      m_cmd_short(cmd_short),
      m_sub_cmd(sub_cmd),
      m_help_extra(help_txt) {}

   /*!
    * Can we parse subcommands for this command?
    */
   bool has_subcommand() const { return m_sub_cmd.size() > 0; }

   /*!
    * Return the short help message
    */
   std::string short_help() const;

   /*!
    * Return the detailed help message
    */
   std::string help() const;

private:
   /*!
    * Called by help() to add command line options to the full help text
    */
   void add_options(std::string &cmd_line, cmd_key_t &key, unsigned int indent) const;

   const unsigned int INDENT      = 4;      //!< Number of spaces to indent when listing subcommands
   const unsigned int WRAP_MARGIN = 80;     //!< Number of characters per line before word wrapping

   std::string              m_cmd_long;     //!< Command string (long version)
   std::string              m_cmd_short;    //!< Command string (short version)
   std::vector<std::string> m_sub_cmd;      //!< Subcommands which are valid for this command, in the
                                            //!< same order that they should be displayed in the help
   std::string              m_help_extra;   //!< Optional extra help text above the options
   mutable std::string      m_help_full;    //!< The full text of the detailed help for this command.
                                            //!< Mutable because it is lazy evaluated when we call help()
};



/*!
 * Map short and long command names to Protocol Buffer enum values
 */
const cmdLookup_t cmdLookup = {
   { "admin",                   AdminCmd::CMD_ADMIN },
   { "ad",                      AdminCmd::CMD_ADMIN },
   { "archivefile",             AdminCmd::CMD_ARCHIVEFILE },
   { "af",                      AdminCmd::CMD_ARCHIVEFILE },
   { "archiveroute",            AdminCmd::CMD_ARCHIVEROUTE },
   { "ar",                      AdminCmd::CMD_ARCHIVEROUTE },
   { "drive",                   AdminCmd::CMD_DRIVE },
   { "dr",                      AdminCmd::CMD_DRIVE },
   { "failedrequest",           AdminCmd::CMD_FAILEDREQUEST },
   { "fr",                      AdminCmd::CMD_FAILEDREQUEST },
   { "groupmountrule",          AdminCmd::CMD_GROUPMOUNTRULE },
   { "gmr",                     AdminCmd::CMD_GROUPMOUNTRULE },
   { "listpendingarchives",     AdminCmd::CMD_LISTPENDINGARCHIVES },
   { "lpa",                     AdminCmd::CMD_LISTPENDINGARCHIVES },
   { "listpendingretrieves",    AdminCmd::CMD_LISTPENDINGRETRIEVES },
   { "lpr",                     AdminCmd::CMD_LISTPENDINGRETRIEVES },
   { "logicallibrary",          AdminCmd::CMD_LOGICALLIBRARY },
   { "ll",                      AdminCmd::CMD_LOGICALLIBRARY },
   { "mediatype",               AdminCmd::CMD_MEDIATYPE },
   { "mt",                      AdminCmd::CMD_MEDIATYPE },
   { "mountpolicy",             AdminCmd::CMD_MOUNTPOLICY },
   { "mp",                      AdminCmd::CMD_MOUNTPOLICY },
   { "repack",                  AdminCmd::CMD_REPACK },
   { "re",                      AdminCmd::CMD_REPACK },
   { "requestermountrule",      AdminCmd::CMD_REQUESTERMOUNTRULE },
   { "rmr",                     AdminCmd::CMD_REQUESTERMOUNTRULE },
   { "showqueues",              AdminCmd::CMD_SHOWQUEUES },
   { "sq",                      AdminCmd::CMD_SHOWQUEUES },
   { "storageclass",            AdminCmd::CMD_STORAGECLASS },
   { "sc",                      AdminCmd::CMD_STORAGECLASS },
   { "tape",                    AdminCmd::CMD_TAPE },
   { "ta",                      AdminCmd::CMD_TAPE },
   { "tapefile",                AdminCmd::CMD_TAPEFILE },
   { "tf",                      AdminCmd::CMD_TAPEFILE },
   { "tapepool",                AdminCmd::CMD_TAPEPOOL },
   { "tp",                      AdminCmd::CMD_TAPEPOOL },
   { "disksystem",              AdminCmd::CMD_DISKSYSTEM },
   { "ds",                      AdminCmd::CMD_DISKSYSTEM },
   { "virtualorganization",     AdminCmd::CMD_VIRTUALORGANIZATION },
   { "vo",                      AdminCmd::CMD_VIRTUALORGANIZATION },
   { "version",                 AdminCmd::CMD_VERSION},
   { "v",                 AdminCmd::CMD_VERSION},
};



/*!
 * Map subcommand names to Protocol Buffer enum values
 */
const subcmdLookup_t subcmdLookup = {
   { "add",                     AdminCmd::SUBCMD_ADD },
   { "ch",                      AdminCmd::SUBCMD_CH },
   { "err",                     AdminCmd::SUBCMD_ERR },
   { "label",                   AdminCmd::SUBCMD_LABEL },
   { "ls",                      AdminCmd::SUBCMD_LS },
   { "reclaim",                 AdminCmd::SUBCMD_RECLAIM },
   { "retry",                   AdminCmd::SUBCMD_RETRY },
   { "rm",                      AdminCmd::SUBCMD_RM },
   { "show",                    AdminCmd::SUBCMD_SHOW },
   { "up",                      AdminCmd::SUBCMD_UP },
   { "down",                    AdminCmd::SUBCMD_DOWN },
};



/*!
 * Map boolean options to Protocol Buffer enum values
 */
const std::map<std::string, OptionBoolean::Key> boolOptions = {
   // Boolean options
   { "--all",                   OptionBoolean::ALL },
   { "--disabled",              OptionBoolean::DISABLED },
   { "--encrypted",             OptionBoolean::ENCRYPTED },
   { "--force",                 OptionBoolean::FORCE },
   { "--full",                  OptionBoolean::FULL },
   { "--readonly",              OptionBoolean::READ_ONLY },

   // hasOption options
   { "--checkchecksum",         OptionBoolean::CHECK_CHECKSUM },
   { "--disabledtape",		OptionBoolean::DISABLED },
   { "--extended",              OptionBoolean::EXTENDED },
   { "--justarchive",           OptionBoolean::JUSTARCHIVE },
   { "--justmove",              OptionBoolean::JUSTMOVE },
   { "--justaddcopies",         OptionBoolean::JUSTADDCOPIES },
   { "--justretrieve",          OptionBoolean::JUSTRETRIEVE },
   { "--log",                   OptionBoolean::SHOW_LOG_ENTRIES },
   { "--lookupnamespace",       OptionBoolean::LOOKUP_NAMESPACE },
   { "--summary",               OptionBoolean::SUMMARY }
};



/*!
 * Map integer options to Protocol Buffer enum values
 */
const std::map<std::string, OptionUInt64::Key> uint64Options = {
   { "--archivepriority",       OptionUInt64::ARCHIVE_PRIORITY },
   { "--capacity",              OptionUInt64::CAPACITY },
   { "--copynb",                OptionUInt64::COPY_NUMBER },
   { "--firstfseq",             OptionUInt64::FIRST_FSEQ },
   { "--gid",                   OptionUInt64::GID },
   { "--id",                    OptionUInt64::ARCHIVE_FILE_ID },
   { "--lastfseq",              OptionUInt64::LAST_FSEQ },
   { "--maxdrivesallowed",      OptionUInt64::MAX_DRIVES_ALLOWED },
   { "--maxlpos",               OptionUInt64::MAX_LPOS },
   { "--minarchiverequestage",  OptionUInt64::MIN_ARCHIVE_REQUEST_AGE },
   { "--minlpos",               OptionUInt64::MIN_LPOS },
   { "--minretrieverequestage", OptionUInt64::MIN_RETRIEVE_REQUEST_AGE },
   { "--nbfiles",               OptionUInt64::NUMBER_OF_FILES },
   { "--nbwraps",               OptionUInt64::NUMBER_OF_WRAPS },
   { "--partial",               OptionUInt64::PARTIAL }, 
   { "--partialtapesnumber",    OptionUInt64::PARTIAL_TAPES_NUMBER },
   { "--primarydensitycode",    OptionUInt64::PRIMARY_DENSITY_CODE },
   { "--retrievepriority",      OptionUInt64::RETRIEVE_PRIORITY },
   { "--secondarydensitycode",  OptionUInt64::SECONDARY_DENSITY_CODE },
   { "--size",                  OptionUInt64::FILE_SIZE },
   { "--refreshinterval",       OptionUInt64::REFRESH_INTERVAL },
   { "--targetedfreespace",     OptionUInt64::TARGETED_FREE_SPACE },
   { "--sleeptime",             OptionUInt64::SLEEP_TIME },
   { "--uid",                   OptionUInt64::OWNER_UID }
};



/*!
 * Map string options to Protocol Buffer enum values
 */
const std::map<std::string, OptionString::Key> strOptions = {
   { "--bufferurl",             OptionString::BUFFERURL }, 
   { "--cartridge",             OptionString::CARTRIDGE },
   { "--comment",               OptionString::COMMENT },
   { "--diskid",                OptionString::DISKID },
   { "--drive",                 OptionString::DRIVE },
   { "--encryptionkeyname",     OptionString::ENCRYPTION_KEY_NAME },
   { "--file",                  OptionString::FILENAME },
   { "--hostname",              OptionString::HOSTNAME },
   { "--input",                 OptionString::INPUT },
   { "--instance",              OptionString::INSTANCE },
   { "--logicallibrary",        OptionString::LOGICAL_LIBRARY },
   { "--mediatype",             OptionString::MEDIA_TYPE },
   { "--mountpolicy",           OptionString::MOUNT_POLICY },
   { "--output",                OptionString::OUTPUT },
   { "--path",                  OptionString::PATH },
   { "--storageclass",          OptionString::STORAGE_CLASS },
   { "--supply",                OptionString::SUPPLY },
   { "--tapepool",              OptionString::TAPE_POOL },
   { "--username",              OptionString::USERNAME },
   { "--vendor",                OptionString::VENDOR },
   { "--vid",                   OptionString::VID },
   { "--vo",                    OptionString::VO },
   { "--disksystem",            OptionString::DISK_SYSTEM },
   { "--fileregexp",            OptionString::FILE_REGEXP },
   { "--freespacequeryurl",     OptionString::FREE_SPACE_QUERY_URL },
   { "--reason",                OptionString::REASON }
};



/*!
 * Map string list options to Protocol Buffer enum values
 */
const std::map<std::string, OptionStrList::Key> strListOptions = {
   { "--fidfile",               OptionStrList::FILE_ID },
   { "--vidfile",               OptionStrList::VID }
};



/*!
 * Specify the help text for commands
 */
const std::map<AdminCmd::Cmd, CmdHelp> cmdHelp = {
   { AdminCmd::CMD_ADMIN,                { "admin",                "ad",  { "add", "ch", "rm", "ls" } }},
   { AdminCmd::CMD_ARCHIVEFILE,          { "archivefile",          "af",  { "ls" } }},
   { AdminCmd::CMD_ARCHIVEROUTE,         { "archiveroute",         "ar",  { "add", "ch", "rm", "ls" } }},
   { AdminCmd::CMD_DRIVE,                { "drive",                "dr",  { "up", "down", "ls", "ch", "rm" },
                        "\n  This is a synchronous command that sets and reads back the state of one or\n"
                          "  more drives. The <drive_name> option accepts a regular expression. If the\n"
                          "  --force option is not set, the drives will complete any running mount and\n"
                          "  drives must be in the down state before deleting.\n\n"
                                         }},
   { AdminCmd::CMD_FAILEDREQUEST,        { "failedrequest",        "fr",  { "ls", "show", "retry", "rm" } }},
   { AdminCmd::CMD_GROUPMOUNTRULE,       { "groupmountrule",       "gmr", { "add", "ch", "rm", "ls" } }},
   { AdminCmd::CMD_LISTPENDINGARCHIVES,  { "listpendingarchives",  "lpa", { } }},
   { AdminCmd::CMD_LISTPENDINGRETRIEVES, { "listpendingretrieves", "lpr", { } }},
   { AdminCmd::CMD_LOGICALLIBRARY,       { "logicallibrary",       "ll",  { "add", "ch", "rm", "ls" } }},
   { AdminCmd::CMD_MEDIATYPE,            { "mediatype",            "mt",  { "add", "ch", "rm", "ls" } }},
   { AdminCmd::CMD_MOUNTPOLICY,          { "mountpolicy",          "mp",  { "add", "ch", "rm", "ls" } }},
   { AdminCmd::CMD_REPACK,               { "repack",               "re",  { "add", "rm", "ls", "err" },
			 "\n  This command allows to manage repack requests.\n\n"
			   "  Submit a repack request by using the \"add\" subcommand :\n"
			   "   * Specify the vid (--vid option) or all the vids to repack by giving a file path to the --vidfile option.\n"
			   "   * If the --bufferURL option is set, it will overwrite the default one. It should respect the following format : root://eosinstance//path/to/repack/buffer.\n"
			   "     The default bufferURL is set in the CTA frontend configuration file.\n"
			   "   * If the --justmove option is set, the files located on the tape to repack will be migrated on one or multiple tapes.\n"
			   "     If the --justaddcopies option is set, new (or missing) copies (as defined by the storage class) of the files located on the tape to repack will be created and migrated.\n"
			   "     By default, CTA will migrate AND add new (or missing) copies (as defined by the storage class) of the files located on the tape to repack.\n"
                           "   * The --mountpolicy option allows to give a specific mount policy that will be applied to the repack subrequests (retrieve and archive requests).\n"
			   "     By default, a hardcoded mount policy is applied (every request priorities and minimum request ages = 1).\n"
			   "   * If the --disabledtape flag is set, the tape to repack will be mounted for retrieval even if it is disabled."
					"\n\n" 
					 }},
   { AdminCmd::CMD_REQUESTERMOUNTRULE,   { "requestermountrule",   "rmr", { "add", "ch", "rm", "ls" } }},
   { AdminCmd::CMD_SHOWQUEUES,           { "showqueues",           "sq",  { } }},
   { AdminCmd::CMD_STORAGECLASS,         { "storageclass",         "sc",  { "add", "ch", "rm", "ls" } }},
   { AdminCmd::CMD_TAPE,                 { "tape",                 "ta",  { "add", "ch", "rm", "reclaim", "ls", "label" } }},
   { AdminCmd::CMD_TAPEFILE,             { "tapefile",             "tf",  { "ls" } }},
   { AdminCmd::CMD_TAPEPOOL,             { "tapepool",             "tp",  { "add", "ch", "rm", "ls" } }},
   { AdminCmd::CMD_DISKSYSTEM,           { "disksystem",           "ds",  { "add", "ch", "rm", "ls" },
			  "\n  This command allows to manage disk systems (useful for the backpressure).\n"
			    "  A disksystem contains informations in order to query EOS for the free space remaining in a specific folder.\n\n"
			    "  Before mounting for a Retrieval, the destination URL of each file to retrieve will be pattern-matched in order to identify the corresponding disksystem.\n"
			    "  The matched disksystem will give informations to query the free space of the correct EOS space in order to backpressure if not enough free space is available.\n\n"
			    "  Add a disksystem by using the \"add\" subcommand :\n"
			    "   * Specify the name (--disksystem) of the disk system. This name must be unique as it is the identifier of the disk system\n"
			    "   * Specify the file regex pattern (--fileregexp option) in order to assign a disk system name to a file to retrieve according to the destinationURL of this file\n"
			    "   Example : If the disksystem's fileregexp is ^root://eos_instance//eos/cta(.*)eos.space=spinners, a file to retrieve with the destination URL\n"
			    "   root://eos_instance//eos/cta/myfile?eos.lfn=fxid:7&eos.space=spinners will then match the corresponding disksystem\n"
			    "   * Specify the informations to query the EOS free space by using the --freespacequeryurl\n"
			    "   It should have the following format : eos:name_of_eos_instance:name_of_eos_space. Example : eos:ctaeos:spinners\n"
			    "   * The refresh interval (--refreshinterval), in seconds, specify how long the queried free space will be use.\n"
			    "   * The targeted free space (--targetedfreespace) based on the free space update latency (inherent to the file system and induced by the refresh  interval)\n"
			    "   and the expected external bandwidth from sources external to CTA.\n"
			    "   * The sleep time (--sleeptime) in seconds will tell the queue containing the Retrieve jobs to sleep so that it will not be popped during this amount of time.\n"
			    "   If the targeted free space is reach, the queue will sleep during this amount of seconds."
			    "\n\n"
					 }},
   { AdminCmd::CMD_VIRTUALORGANIZATION,  { "virtualorganization",   "vo",  { "add", "ch", "rm", "ls" } }},
   { AdminCmd::CMD_VERSION,              { "version",           "v",  { } }},
};



/*
 * Enumerate options
 */
const Option opt_all                  { Option::OPT_FLAG, "--all",                   "-a",   "" };
const Option opt_archivefileid        { Option::OPT_UINT, "--id",                    "-I",   " <archive_file_id>" };
const Option opt_archivepriority      { Option::OPT_UINT, "--archivepriority",       "--ap", " <priority_value>" };
const Option opt_bufferurl            { Option::OPT_STR,  "--bufferurl",             "-b",   " <buffer URL>" };
const Option opt_capacity             { Option::OPT_UINT, "--capacity",              "-c",   " <capacity_in_bytes>" };
const Option opt_cartridge            { Option::OPT_STR,  "--cartridge",             "-t",   " <cartridge>" };
const Option opt_checkchecksum        { Option::OPT_FLAG, "--checkchecksum",         "-c",   "" };
const Option opt_comment              { Option::OPT_STR,  "--comment",               "-m",   " <\"comment\">" };
const Option opt_copynb               { Option::OPT_UINT, "--copynb",                "-c",   " <copy_number>" };
const Option opt_disabled             { Option::OPT_BOOL, "--disabled",              "-d",   " <\"true\" or \"false\">" };
const Option opt_diskid               { Option::OPT_STR,  "--diskid",                "-d",   " <disk_id>" };
const Option opt_drivename            { Option::OPT_STR,  "--drive",                 "-d",   " <drive_name>" };
const Option opt_drivename_cmd        { Option::OPT_CMD,  "--drive",                 "",     "<drive_name>" };
const Option opt_encrypted            { Option::OPT_BOOL, "--encrypted",             "-e",   " <\"true\" or \"false\">" };
const Option opt_encryptionkeyname    { Option::OPT_STR,  "--encryptionkeyname",     "-k",   " <encryption_key_name>" };
const Option opt_extended             { Option::OPT_FLAG, "--extended",              "-x",   "" };
const Option opt_fidfile              { Option::OPT_STR_LIST, "--fidfile",           "-i",   " <filename>" };
const Option opt_filename             { Option::OPT_STR,  "--file",                  "-f",   " <filename>" };
const Option opt_firstfseq            { Option::OPT_UINT, "--firstfseq",             "-f",   " <first_fseq>" };
const Option opt_force                { Option::OPT_BOOL, "--force",                 "-f",   " <\"true\" or \"false\">" };
const Option opt_force_flag           { Option::OPT_FLAG, "--force",                 "-f",   "" };
const Option opt_gid                  { Option::OPT_UINT, "--gid",                   "-g",   " <group_id>" };
const Option opt_hostname_alias       { Option::OPT_STR,  "--name",                  "-n",   " <host_name>",
                                        "--hostname" };
const Option opt_input                { Option::OPT_STR,  "--input",                 "-i",   " <\"zero\" or \"urandom\">" };
const Option opt_instance             { Option::OPT_STR,  "--instance",              "-i",   " <instance_name>" };
const Option opt_justarchive          { Option::OPT_FLAG, "--justarchive",           "-a",   "" };
const Option opt_justmove             { Option::OPT_FLAG, "--justmove",              "-m",   "" };
const Option opt_justaddcopies        { Option::OPT_FLAG, "--justaddcopies",         "-a",   "" };
const Option opt_justretrieve         { Option::OPT_FLAG, "--justretrieve",          "-r",   "" };
const Option opt_lastfseq             { Option::OPT_UINT, "--lastfseq",              "-l",   " <last_fseq>" };
const Option opt_log                  { Option::OPT_FLAG, "--log",                   "-l",   "" };
const Option opt_logicallibrary       { Option::OPT_STR,  "--logicallibrary",        "-l",   " <logical_library_name>" };
const Option opt_logicallibrary_alias { Option::OPT_STR,  "--name",                  "-n",   " <logical_library_name>",
                                        "--logicallibrary" };
const Option opt_lookupns             { Option::OPT_FLAG, "--lookupnamespace",       "-l",   "" };
const Option opt_maxdrivesallowed     { Option::OPT_UINT, "--maxdrivesallowed",      "-d",   " <max_drives_allowed>" };
const Option opt_maxlpos              { Option::OPT_UINT, "--maxlpos",               "-maxl",   " <maximum_longitudinal_position>" };
const Option opt_mediatype            { Option::OPT_STR,  "--mediatype",             "--mt", " <media_type_name>" };
const Option opt_mediatype_alias      { Option::OPT_STR,  "--name",                  "-n",   " <media_type_name>",
                                        "--mediatype" };
const Option opt_minarchiverequestage { Option::OPT_UINT, "--minarchiverequestage",  "--aa", " <min_request_age>" };
const Option opt_minlpos              { Option::OPT_UINT, "--minlpos",               "-minl",   " <minimum_longitudinal_position>" };
const Option opt_minretrieverequestage{ Option::OPT_UINT, "--minretrieverequestage", "--ra", " <min_request_age>" };
const Option opt_mountpolicy          { Option::OPT_STR,  "--mountpolicy",           "-u",   " <mount_policy_name>" };
const Option opt_mountpolicy_alias    { Option::OPT_STR,  "--name",                  "-n",   " <mount_policy_name>",
                                        "--mountpolicy" };
const Option opt_number_of_files      { Option::OPT_UINT, "--nbfiles",               "-n",   " <number_of_files_per_tape>" };
const Option opt_number_of_files_alias{ Option::OPT_UINT, "--number",                "-n",   " <number_of_files>",
                                        "--nbfiles" };
const Option opt_number_of_wraps      { Option::OPT_UINT, "--nbwraps",               "-w",   " <number_of_wraps>" };
const Option opt_output               { Option::OPT_STR,  "--output",                "-o",   " <\"null\" or output_dir>" };
const Option opt_owner_uid            { Option::OPT_UINT, "--uid",                   "-u",   " <owner_uid>" };
const Option opt_partialfiles         { Option::OPT_UINT, "--partial",               "-p",   " <number_of_files_per_tape>" };
const Option opt_partialtapes         { Option::OPT_UINT, "--partialtapesnumber",    "-p",   " <number_of_partial_tapes>" };
const Option opt_path                 { Option::OPT_STR,  "--path",                  "-p",   " <full_path>" };
const Option opt_primarydensitycode   { Option::OPT_UINT,  "--primarydensitycode",    "-p",   " <primary_density_code>" };
const Option opt_retrievepriority     { Option::OPT_UINT, "--retrievepriority",      "--rp", " <priority_value>" };
const Option opt_secondarydensitycode { Option::OPT_UINT, "--secondarydensitycode",  "-s",   " <secondary_density_code>" };
const Option opt_size                 { Option::OPT_UINT, "--size",                  "-s",   " <file_size>" };
const Option opt_storageclass         { Option::OPT_STR,  "--storageclass",          "-s",   " <storage_class_name>" };
const Option opt_storageclass_alias   { Option::OPT_STR,  "--name",                  "-n",   " <storage_class_name>",
                                        "--storageclass" };
const Option opt_summary              { Option::OPT_FLAG, "--summary",               "-S",   "" };
const Option opt_supply               { Option::OPT_STR,  "--supply",                "-s",   " <supply_value>" };
const Option opt_tapepool             { Option::OPT_STR,  "--tapepool",              "-t",   " <tapepool_name>" };
const Option opt_tapepool_alias       { Option::OPT_STR,  "--name",                  "-n",   " <tapepool_name>",
                                        "--tapepool" };
const Option opt_username             { Option::OPT_STR,  "--username",              "-u",   " <user_name>" };
const Option opt_username_alias       { Option::OPT_STR,  "--name",                  "-n",   " <user_name>",
                                        "--username" };
const Option opt_vendor               { Option::OPT_STR,  "--vendor",                "--ve", " <vendor>" };
const Option opt_vid                  { Option::OPT_STR,  "--vid",                   "-v",   " <vid>" };
const Option opt_vo                   { Option::OPT_STR,  "--vo",                    "--vo", " <vo>" };
const Option opt_vidfile              { Option::OPT_STR_LIST, "--vidfile",           "-f",   " <filename>" };
const Option opt_full                 { Option::OPT_BOOL, "--full",                  "-f",   " <\"true\" or \"false\">" };
const Option opt_readonly             { Option::OPT_BOOL, "--readonly",              "-r",   " <\"true\" or \"false\">" };
const Option opt_disabled_tape        { Option::OPT_FLAG, "--disabledtape",          "-d",   ""};

const Option opt_disksystem           { Option::OPT_STR,  "--disksystem",            "-n", " <disk_system_name>" };
const Option opt_file_regexp          { Option::OPT_STR,  "--fileregexp",            "-r", " <file_regexp>" };
const Option opt_free_space_query_url { Option::OPT_STR,  "--freespacequeryurl",     "-u", " <free_space_query_url>" };
const Option opt_refresh_interval     { Option::OPT_UINT,  "--refreshinterval",      "-i", " <refresh_intreval>" };
const Option opt_targeted_free_space  { Option::OPT_UINT,  "--targetedfreespace",    "-f", " <targeted_free_space>" };
const Option opt_sleep_time           { Option::OPT_UINT,  "--sleeptime",            "-s", " <sleep time in s>" };
const Option opt_reason               { Option::OPT_STR,   "--reason",               "-r", " <reason_status_change>" };

/*!
 * Map valid options to commands
 */
const std::map<cmd_key_t, cmd_val_t> cmdOptions = {
   {{ AdminCmd::CMD_ADMIN,                AdminCmd::SUBCMD_ADD   }, { opt_username, opt_comment }},
   {{ AdminCmd::CMD_ADMIN,                AdminCmd::SUBCMD_CH    }, { opt_username, opt_comment }},
   {{ AdminCmd::CMD_ADMIN,                AdminCmd::SUBCMD_RM    }, { opt_username }},
   {{ AdminCmd::CMD_ADMIN,                AdminCmd::SUBCMD_LS    }, { }},
   /*----------------------------------------------------------------------------------------------------*/
   {{ AdminCmd::CMD_ARCHIVEFILE,          AdminCmd::SUBCMD_LS    },
      { opt_archivefileid.optional(), opt_diskid.optional(), opt_copynb.optional(),
        opt_vid.optional(), opt_tapepool.optional(), opt_owner_uid.optional(), opt_gid.optional(),
        opt_storageclass.optional(), opt_path.optional(), opt_instance.optional(), opt_all.optional(),
        opt_summary.optional() }},
   /*----------------------------------------------------------------------------------------------------*/
   {{ AdminCmd::CMD_ARCHIVEROUTE,         AdminCmd::SUBCMD_ADD   },
      { opt_storageclass, opt_copynb, opt_tapepool, opt_comment }},
   {{ AdminCmd::CMD_ARCHIVEROUTE,         AdminCmd::SUBCMD_CH    },
      { opt_storageclass, opt_copynb, opt_tapepool.optional(), opt_comment.optional() }},
   {{ AdminCmd::CMD_ARCHIVEROUTE,         AdminCmd::SUBCMD_RM    }, { opt_storageclass, opt_copynb }},
   {{ AdminCmd::CMD_ARCHIVEROUTE,         AdminCmd::SUBCMD_LS    }, { }},
   /*----------------------------------------------------------------------------------------------------*/
   {{ AdminCmd::CMD_DRIVE,                AdminCmd::SUBCMD_UP    }, { opt_drivename_cmd, opt_reason.optional() }},
   {{ AdminCmd::CMD_DRIVE,                AdminCmd::SUBCMD_DOWN  }, { opt_drivename_cmd, opt_reason, opt_force_flag.optional() }},
   {{ AdminCmd::CMD_DRIVE,                AdminCmd::SUBCMD_LS    }, { opt_drivename.optional() }},
   {{ AdminCmd::CMD_DRIVE,                AdminCmd::SUBCMD_RM    }, { opt_drivename_cmd, opt_force_flag.optional() }},
   {{ AdminCmd::CMD_DRIVE,                AdminCmd::SUBCMD_CH    }, { opt_drivename_cmd, opt_comment }},
   /*----------------------------------------------------------------------------------------------------*/
   {{ AdminCmd::CMD_FAILEDREQUEST,        AdminCmd::SUBCMD_LS    },
      { opt_justarchive.optional(), opt_justretrieve.optional(), opt_tapepool.optional(),
        opt_vid.optional(), opt_log.optional(), opt_summary.optional() }},
   {{ AdminCmd::CMD_FAILEDREQUEST,        AdminCmd::SUBCMD_SHOW  }, { opt_copynb.optional() }},
   {{ AdminCmd::CMD_FAILEDREQUEST,        AdminCmd::SUBCMD_RETRY }, { opt_copynb.optional() }},
   {{ AdminCmd::CMD_FAILEDREQUEST,        AdminCmd::SUBCMD_RM    }, { opt_copynb.optional() }},
   /*----------------------------------------------------------------------------------------------------*/
   {{ AdminCmd::CMD_GROUPMOUNTRULE,       AdminCmd::SUBCMD_ADD   },
      { opt_instance, opt_username_alias, opt_mountpolicy, opt_comment }},
   {{ AdminCmd::CMD_GROUPMOUNTRULE,       AdminCmd::SUBCMD_CH    },
      { opt_instance, opt_username_alias, opt_mountpolicy.optional(), opt_comment.optional() }},
   {{ AdminCmd::CMD_GROUPMOUNTRULE,       AdminCmd::SUBCMD_RM    }, { opt_instance, opt_username_alias }},
   {{ AdminCmd::CMD_GROUPMOUNTRULE,       AdminCmd::SUBCMD_LS    }, { }},
   /*----------------------------------------------------------------------------------------------------*/
   {{ AdminCmd::CMD_LISTPENDINGARCHIVES,  AdminCmd::SUBCMD_NONE  },
      { opt_tapepool.optional(), opt_extended.optional() }},
   /*----------------------------------------------------------------------------------------------------*/
   {{ AdminCmd::CMD_LISTPENDINGRETRIEVES, AdminCmd::SUBCMD_NONE  },
      { opt_vid.optional(), opt_extended.optional() }},
   /*----------------------------------------------------------------------------------------------------*/
   {{ AdminCmd::CMD_LOGICALLIBRARY,       AdminCmd::SUBCMD_ADD   },
      { opt_logicallibrary_alias, opt_disabled.optional(), opt_comment }},
   {{ AdminCmd::CMD_LOGICALLIBRARY,       AdminCmd::SUBCMD_CH    },
      { opt_logicallibrary_alias, opt_disabled.optional(), opt_comment.optional() }},
   {{ AdminCmd::CMD_LOGICALLIBRARY,       AdminCmd::SUBCMD_RM    }, { opt_logicallibrary_alias }},
   {{ AdminCmd::CMD_LOGICALLIBRARY,       AdminCmd::SUBCMD_LS    }, { }},
   /*----------------------------------------------------------------------------------------------------*/
   {{ AdminCmd::CMD_MEDIATYPE,            AdminCmd::SUBCMD_ADD   },
      { opt_mediatype_alias, opt_cartridge, opt_capacity, opt_primarydensitycode.optional(), opt_secondarydensitycode.optional(), opt_number_of_wraps.optional(), opt_minlpos.optional(), opt_maxlpos.optional(), opt_comment }},
   {{ AdminCmd::CMD_MEDIATYPE,            AdminCmd::SUBCMD_CH    },
      { opt_mediatype_alias, opt_cartridge.optional(), opt_capacity.optional(), opt_primarydensitycode.optional(), opt_secondarydensitycode.optional(), opt_number_of_wraps.optional(), opt_minlpos.optional(), opt_maxlpos.optional(),opt_comment.optional() }},
   {{ AdminCmd::CMD_MEDIATYPE,            AdminCmd::SUBCMD_RM    }, { opt_mediatype_alias }},
   {{ AdminCmd::CMD_MEDIATYPE,            AdminCmd::SUBCMD_LS    }, { }},
   /*----------------------------------------------------------------------------------------------------*/
   {{ AdminCmd::CMD_MOUNTPOLICY,          AdminCmd::SUBCMD_ADD   },
      { opt_mountpolicy_alias, opt_archivepriority, opt_minarchiverequestage, opt_retrievepriority,
        opt_minretrieverequestage, opt_maxdrivesallowed, opt_comment }},
   {{ AdminCmd::CMD_MOUNTPOLICY,          AdminCmd::SUBCMD_CH    },
      { opt_mountpolicy_alias, opt_archivepriority.optional(), opt_minarchiverequestage.optional(),
        opt_retrievepriority.optional(), opt_minretrieverequestage.optional(), opt_maxdrivesallowed.optional(),
        opt_comment.optional() }},
   {{ AdminCmd::CMD_MOUNTPOLICY,          AdminCmd::SUBCMD_RM    }, { opt_mountpolicy_alias }},
   {{ AdminCmd::CMD_MOUNTPOLICY,          AdminCmd::SUBCMD_LS    }, { }},
   /*----------------------------------------------------------------------------------------------------*/
   {{ AdminCmd::CMD_REPACK,               AdminCmd::SUBCMD_ADD   },
      { opt_vid.optional(), opt_vidfile.optional(), opt_bufferurl.optional(), opt_justmove.optional(), opt_justaddcopies.optional(), opt_mountpolicy, opt_disabled_tape.optional() }},
   {{ AdminCmd::CMD_REPACK,               AdminCmd::SUBCMD_RM    }, { opt_vid }},
   {{ AdminCmd::CMD_REPACK,               AdminCmd::SUBCMD_LS    }, { opt_vid.optional() }},
   {{ AdminCmd::CMD_REPACK,               AdminCmd::SUBCMD_ERR   }, { opt_vid }},
   /*----------------------------------------------------------------------------------------------------*/
   {{ AdminCmd::CMD_REQUESTERMOUNTRULE,   AdminCmd::SUBCMD_ADD   },
      { opt_instance, opt_username_alias, opt_mountpolicy, opt_comment }},
   {{ AdminCmd::CMD_REQUESTERMOUNTRULE,   AdminCmd::SUBCMD_CH    },
      { opt_instance, opt_username_alias, opt_mountpolicy.optional(), opt_comment.optional() }},
   {{ AdminCmd::CMD_REQUESTERMOUNTRULE,   AdminCmd::SUBCMD_RM    }, { opt_instance, opt_username_alias }},
   {{ AdminCmd::CMD_REQUESTERMOUNTRULE,   AdminCmd::SUBCMD_LS    }, { }},
   /*----------------------------------------------------------------------------------------------------*/
   {{ AdminCmd::CMD_SHOWQUEUES,           AdminCmd::SUBCMD_NONE  }, { }},
   /*----------------------------------------------------------------------------------------------------*/
   {{ AdminCmd::CMD_STORAGECLASS,         AdminCmd::SUBCMD_ADD   },
      { opt_storageclass_alias, opt_copynb, opt_vo ,opt_comment }},
   {{ AdminCmd::CMD_STORAGECLASS,         AdminCmd::SUBCMD_CH    },
      { opt_storageclass_alias, opt_copynb.optional(), opt_vo.optional(), opt_comment.optional() }},
   {{ AdminCmd::CMD_STORAGECLASS,         AdminCmd::SUBCMD_RM    }, { opt_storageclass_alias }},
   {{ AdminCmd::CMD_STORAGECLASS,         AdminCmd::SUBCMD_LS    }, { }},
   /*----------------------------------------------------------------------------------------------------*/
   {{ AdminCmd::CMD_TAPE,                 AdminCmd::SUBCMD_ADD   },
      { opt_vid, opt_mediatype, opt_vendor, opt_logicallibrary, opt_tapepool, opt_disabled, opt_full, opt_readonly,
        opt_comment.optional() }},
   {{ AdminCmd::CMD_TAPE,                 AdminCmd::SUBCMD_CH    },
      { opt_vid, opt_mediatype.optional(), opt_vendor.optional(), opt_logicallibrary.optional(),
        opt_tapepool.optional(), opt_encryptionkeyname.optional(), opt_disabled.optional(),
        opt_full.optional(), opt_readonly.optional(), opt_comment.optional() }},
   {{ AdminCmd::CMD_TAPE,                 AdminCmd::SUBCMD_RM    }, { opt_vid }},
   {{ AdminCmd::CMD_TAPE,                 AdminCmd::SUBCMD_RECLAIM }, { opt_vid }},
   {{ AdminCmd::CMD_TAPE,                 AdminCmd::SUBCMD_LS    },
      { opt_vid.optional(), opt_mediatype.optional(), opt_vendor.optional(),
        opt_logicallibrary.optional(), opt_tapepool.optional(), opt_vo.optional(), opt_capacity.optional(),
        opt_disabled.optional(), opt_full.optional(), opt_readonly.optional(), opt_fidfile.optional(), opt_all.optional() }},
   {{ AdminCmd::CMD_TAPE,                 AdminCmd::SUBCMD_LABEL },
      { opt_vid, opt_force.optional() }},
   /*----------------------------------------------------------------------------------------------------*/
   {{ AdminCmd::CMD_TAPEFILE,             AdminCmd::SUBCMD_LS    }, { opt_vid, opt_lookupns.optional() }},
   /*----------------------------------------------------------------------------------------------------*/
   {{ AdminCmd::CMD_TAPEPOOL,             AdminCmd::SUBCMD_ADD   },
      { opt_tapepool_alias, opt_vo, opt_partialtapes, opt_encrypted, opt_supply.optional(), opt_comment }},
   {{ AdminCmd::CMD_TAPEPOOL,             AdminCmd::SUBCMD_CH    },
      { opt_tapepool_alias, opt_vo.optional(), opt_partialtapes.optional(), opt_encrypted.optional(),
        opt_supply.optional(), opt_comment.optional() }},
   {{ AdminCmd::CMD_TAPEPOOL,             AdminCmd::SUBCMD_RM    }, { opt_tapepool_alias }},
   {{ AdminCmd::CMD_TAPEPOOL,             AdminCmd::SUBCMD_LS    }, { }},
   /*----------------------------------------------------------------------------------------------------*/
   {{ AdminCmd::CMD_DISKSYSTEM,           AdminCmd::SUBCMD_ADD   },
      { opt_disksystem, opt_file_regexp, opt_free_space_query_url, opt_refresh_interval, opt_targeted_free_space, opt_sleep_time,
        opt_comment }},
   {{ AdminCmd::CMD_DISKSYSTEM,           AdminCmd::SUBCMD_CH    },
      { opt_disksystem, opt_file_regexp.optional(), opt_free_space_query_url.optional(), opt_refresh_interval.optional(),
        opt_targeted_free_space.optional(), opt_sleep_time.optional(), opt_comment.optional() }},
   {{ AdminCmd::CMD_DISKSYSTEM,           AdminCmd::SUBCMD_RM    }, { opt_disksystem }},
   {{ AdminCmd::CMD_DISKSYSTEM,           AdminCmd::SUBCMD_LS    }, { }},
   {{ AdminCmd::CMD_VIRTUALORGANIZATION,           AdminCmd::SUBCMD_ADD   },
      { opt_vo, opt_comment }},
   {{ AdminCmd::CMD_VIRTUALORGANIZATION,           AdminCmd::SUBCMD_CH   },
      { opt_vo, opt_comment }},
   {{ AdminCmd::CMD_VIRTUALORGANIZATION,           AdminCmd::SUBCMD_RM   },
      { opt_vo }},
   {{ AdminCmd::CMD_VIRTUALORGANIZATION,           AdminCmd::SUBCMD_LS   },
      { }},
   {{ AdminCmd::CMD_VERSION,           AdminCmd::SUBCMD_NONE   }, { }},
};



/*!
 * Validate that all required command line options are present
 *
 * Throws a std::runtime_error if the command is invalid
 */
void validateCmd(const cta::admin::AdminCmd &admincmd);

}} // namespace cta::admin
