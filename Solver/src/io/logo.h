#ifndef LOGO_H
#define LOGO_H

#include <iostream>

// Function to print the logo to any output stream
inline void printLogo(std::ostream& out) {
    out << "╔═════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    out << "║                                                                             ║" << std::endl;
    out << "║    ██████╗██████╗      ███████╗ ██████╗ ██╗    ██╗ ╔██ ███████╗██████╗      ║" << std::endl;
    out << "║   ██╔════╝██╔══██╗     ██╔════╝██╔═══██╗██║    ██║ ║██ ██╔════╝██╔══██╗     ║" << std::endl;
    out << "║   ██║     ██████╔╝ ██  ███████╗██║   ██║██║    █║   ║█ █████╗  ██████╔╝     ║" << std::endl;
    out << "║   ██║     ██╔═══╝      ╚════██║██║   ██║██║    ║█   █║ ██╔══╝  ██╔══██╗     ║" << std::endl;
    out << "║   ╚██████╗██║          ███████║╚██████╔╝╚██████╚█████╝ ███████╗██║  ██║     ║" << std::endl;
    out << "║    ╚═════╝╚═╝          ╚══════╝ ╚═════╝  ╚════╝ ╚═══╝  ╚══════╝╚═╝  ╚═╝     ║" << std::endl;
    out << "║                                                                             ║" << std::endl;
    out << "║              Constraint Satisfaction Problem Solver                         ║" << std::endl;
    out << "║                    By Erwann Esteve & Charles Vielzeuf                      ║" << std::endl;
    out << "║                                                                             ║" << std::endl;
    out << "╚═════════════════════════════════════════════════════════════════════════════╝" << std::endl;
}

#endif // LOGO_H
