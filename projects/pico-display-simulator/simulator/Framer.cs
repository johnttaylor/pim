using System;

namespace Simulator
{
    // Encodes/decodes a 'frame' from an ASCII input stream.
    // Frame:
    //      Start-of-Frame (SOF) character
    //      End-of-Frame   (EOF) character
    //      Escape         (ESC) character -->used to escape am EOF/ESC characters, e.g. embed an <ESC> char in the frame: <ESC><ESC>, embed <EOF>: <ESC><EOF>
    //
    //      Note: SOF character is ignored when in a frame, i.e. the SOF is treated as frame data

    //
    public class Framer
    {
        public char m_sofChar;
        public char m_eofChar;
        public char m_escChar;
        public bool m_sof;
        public bool m_escaping;
        public string m_remainingCharacters;
        public string m_current_frame;

        public Framer(char sof = '^', char eof = ';', char esc = '`')
        {
            m_sofChar = sof;
            m_eofChar = eof;
            m_escChar = esc;
            m_sof = false;
            m_escaping = false;
            m_remainingCharacters = "";
            m_current_frame = "";
        }

        // Returns null if no frame found
        // Successful deframe operation - returns frame contents with the leading/trailing SOF/EOF characters removed
        public string Deframe(string data = "")
        {
            string frameContent = "";
            m_remainingCharacters += data;

            int charactersParsed = 0;
            foreach (char c in m_remainingCharacters)
            {
                charactersParsed++;

                // Searching for Start of Frame
                if (m_sof == false)
                {
                    if (c == m_sofChar)
                    {
                        m_sof = true;
                        frameContent = "";
                    }
                }

                // In a frame 
                else
                {
                    // Trap non-ascii characters
                    if (Char.GetNumericValue(c) > 0x7F)
                    {
                        m_sof = false;
                    }

                    // No escape sequence in progress
                    else if (!m_escaping)
                    {
                        // EOF -->save parsed data and return the frame
                        if (c == m_eofChar)
                        {
                            m_sof = false;
                            m_remainingCharacters = m_remainingCharacters.Substring(charactersParsed);
                            frameContent = m_current_frame + frameContent;
                            m_current_frame = "";
                            return frameContent;
                        }

                        // Not EOF and 'regular' character -->add to frame content
                        if (c != m_escChar)
                        {
                            frameContent += c;
                        }

                        // Start of an escape sequence -->drop the escape character
                        else
                        {
                            m_escaping = true;
                        }
                    }

                    // Escape sequence
                    else
                    {
                        // Store the escaped character
                        frameContent += c;
                        m_escaping = false;
                    }
                }

            }

            // If a get here - a complete frame was NOT found
            m_remainingCharacters = m_remainingCharacters.Substring(charactersParsed);
            m_current_frame += frameContent; ;
            return null;
        }
    }
}