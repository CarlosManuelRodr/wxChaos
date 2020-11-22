
////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>
#include <iomanip>
#include <iostream>


const sf::Uint8 AudioData   = 1;
const sf::Uint8 EndOfStream = 2;


////////////////////////////////////////////////////////////
/// Customized sound stream for acquiring audio data
/// from the network
////////////////////////////////////////////////////////////
class NetworkAudioStream : public sf::SoundStream
{
public :

    ////////////////////////////////////////////////////////////
    /// Default constructor
    ///
    ////////////////////////////////////////////////////////////
    NetworkAudioStream() :
    myOffset     (0),
    myHasFinished(false)
    {
        // Set the sound parameters
        Initialize(1, 44100);
    }

    ////////////////////////////////////////////////////////////
    /// Destructor
    ///
    ////////////////////////////////////////////////////////////
    ~NetworkAudioStream()
    {
        // Close the sockets
        myClient.Close();
        myListener.Close();
    }

    ////////////////////////////////////////////////////////////
    /// Run the server, stream audio data from the client
    ///
    ////////////////////////////////////////////////////////////
    void Start(unsigned short Port)
    {
        if (!myHasFinished)
        {
            // Listen to the given port for incoming connections
            if (!myListener.Listen(Port))
                return;
            std::cout << "Server is listening to port " << Port << ", waiting for connections... " << std::endl;

            // Wait for a connection
            sf::IPAddress ClientAddress;
            myListener.Accept(myClient, &ClientAddress);
            std::cout << "Client connected : " << ClientAddress << std::endl;

            // Start playback
            Play();

            // Start receiving audio data
            ReceiveLoop();
        }
        else
        {
            // Start playback
            Play();
        }
    }

private :

    ////////////////////////////////////////////////////////////
    /// /see SoundStream::OnStart
    ///
    ////////////////////////////////////////////////////////////
    virtual bool OnStart()
    {
        // Reset the playing offset
        myOffset = 0;

        return true;
    }

    ////////////////////////////////////////////////////////////
    /// /see SoundStream::OnGetData
    ///
    ////////////////////////////////////////////////////////////
    virtual bool OnGetData(sf::SoundStream::Chunk& Data)
    {
        // We have reached the end of the buffer and all audio data have been played : we can stop playback
        if ((myOffset == mySamples.size()) && myHasFinished)
            return false;

        // No new data has arrived since last update : wait until we get some
        while ((myOffset == mySamples.size()) && !myHasFinished)
            sf::Sleep(0.01f);

        // Copy samples into a local buffer to avoid synchronization problems
        // (don't forget that we run in two separate threads)
        {
            sf::Lock Lock(myMutex);
            myTempBuffer.assign(mySamples.begin() + myOffset, mySamples.end());
        }

        // Fill audio data to pass to the stream
        Data.Samples   = &myTempBuffer[0];
        Data.NbSamples = myTempBuffer.size();

        // Update the playing offset
        myOffset += myTempBuffer.size();

        return true;
    }

    ////////////////////////////////////////////////////////////
    /// Get audio data from the client until playback is stopped
    ///
    ////////////////////////////////////////////////////////////
    void ReceiveLoop()
    {
        while (!myHasFinished)
        {
            // Get waiting audio data from the network
            sf::Packet PacketIn;
            if (myClient.Receive(PacketIn) != sf::Socket::Done)
                break;

            // Extract the message ID
            sf::Uint8 Id;
            PacketIn >> Id;

            if (Id == AudioData)
            {
                // Extract audio samples from the packet, and append it to our samples buffer
                const sf::Int16* Samples   = reinterpret_cast<const sf::Int16*>(PacketIn.GetData() + 1);
                std::size_t      NbSamples = (PacketIn.GetDataSize() - 1) / sizeof(sf::Int16);

                // Don't forget that the other thread can access the samples array at any time
                // (so we protect any operation on it with the mutex)
                {
                    sf::Lock Lock(myMutex);
                    std::copy(Samples, Samples + NbSamples, std::back_inserter(mySamples));
                }
            }
            else if (Id == EndOfStream)
            {
                // End of stream reached : we stop receiving audio data
                std::cout << "Audio data has been 100% received !" << std::endl;
                myHasFinished = true;
            }
            else
            {
                // Something's wrong...
                std::cout << "Invalid packet received..." << std::endl;
                myHasFinished = true;
            }
        }
    }

    ////////////////////////////////////////////////////////////
    // Member data
    ////////////////////////////////////////////////////////////
    sf::SocketTCP          myListener;
    sf::SocketTCP          myClient;
    sf::Mutex              myMutex;
    std::vector<sf::Int16> mySamples;
    std::vector<sf::Int16> myTempBuffer;
    std::size_t            myOffset;
    bool                   myHasFinished;
};


////////////////////////////////////////////////////////////
/// Launch a server and wait for incoming audio data from
/// a connected client
///
////////////////////////////////////////////////////////////
void DoServer(unsigned short Port)
{
    // Build an audio stream to play sound data as it is received through the network
    NetworkAudioStream AudioStream;
    AudioStream.Start(Port);

    // Loop until the sound playback is finished
    while (AudioStream.GetStatus() != sf::SoundStream::Stopped)
    {
        // Leave some CPU time for other threads
        sf::Sleep(0.1f);
    }

    std::cin.ignore(10000, '\n');

    // Wait until the user presses 'enter' key
    std::cout << "Press enter to replay the sound..." << std::endl;
    std::cin.ignore(10000, '\n');

    // Replay the sound (just to make sure replaying the received data is OK)
    AudioStream.Play();

    // Loop until the sound playback is finished
    while (AudioStream.GetStatus() != sf::SoundStream::Stopped)
    {
        // Leave some CPU time for other threads
        sf::Sleep(0.1f);
    }
}
