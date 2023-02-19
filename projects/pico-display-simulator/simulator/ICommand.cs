using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Simulator
{
    public interface ICommand
    {
        string GetCommandName();
        bool ExecuteCommand( string rawString, List<string> tokenizeString);
    }
}
