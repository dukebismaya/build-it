#include <iomanip>
#include <ios>
#include <iostream>
#include <iterator>
#include <limits>
#include <list>
#include <string>

class Song {
  friend std::ostream &operator<<(std::ostream &os, const Song &s);
  std::string name;
  std::string artist;
  int rating;

public:
  Song() = default;
  Song(std::string name, std::string artist, int rating)
      : name{name}, artist{artist}, rating{rating} {}
  std::string get_name() const { return name; }
  std::string get_artist() const { return artist; }
  int get_rating() const { return rating; }

  bool operator<(const Song &rhs) const { return this->name < rhs.name; }

  bool operator==(const Song &rhs) const { return this->name == rhs.name; }
};

std::ostream &operator<<(std::ostream &os, const Song &s) {
  os << std::setw(20) << std::left << s.name << std::setw(30) << std::left
     << s.artist << std::setw(2) << std::left << s.rating;
  return os;
}

void display_menu() {
  std::cout << "\nF - Play First Song" << std::endl;
  std::cout << "N - Play Next song" << std::endl;
  std::cout << "P - Play Previous song" << std::endl;
  std::cout << "A - Add and play a new Song at current location" << std::endl;
  std::cout << "L - List the current playlist" << std::endl;
  std::cout << "===============================================" << std::endl;
  std::cout << "Enter a selection (Q to quit): ";
}

void play_current_song(const Song &song) {
  std::cout << "Playing: \n";
  std::cout << song;
}

void display_playlist(const std::list<Song> &playlist,
                      const Song &current_song) {
  for (const auto &l : playlist) {
    std::cout << l << "\n";
  }
  std::cout << "Current Song: \n";
  std::cout << current_song;
}

auto is_empty(const std::list<Song> &playlist) -> bool {
  if (playlist.empty()) {
    std::clog << "Playlist is empty. Try to add some songs." << std::endl;
  }
  return playlist.empty();
}

int main() {

  std::list<Song> playlist{{"Bohemian Rhapsody", "Queen", 5},
                           {"Hotel California", "Eagles", 5},
                           {"Imagine", "John Lennon", 5},
                           {"Billie Jean", "Michael Jackson", 5},
                           {"Shape of You", "Ed Sheeran", 4},
                           {"Stairway to Heaven", "Led Zeppelin", 5}};

  std::list<Song>::iterator current_song = playlist.begin();
  bool running{true};
  while (running) {
    display_menu();
    char input{};
    std::cin >> input;
    switch (input) {
    case 'F':
    case 'f': {
      if (is_empty(playlist)) {
        continue;
      }
      current_song = playlist.begin();
      play_current_song(*current_song);
      break;
    }
    case 'N':
    case 'n': {
      if (is_empty(playlist)) {
        continue;
      }
      if (std::next(current_song) == playlist.end()) {
        current_song = playlist.begin();
      } else {
        current_song = std::next(current_song);
      }
      play_current_song(*current_song);
      break;
    }
    case 'P':
    case 'p': {
      if (is_empty(playlist))
        continue;
      if (current_song == playlist.begin()) {
        current_song = std::prev(playlist.end());
      } else {
        current_song = std::prev(current_song);
      }
      play_current_song(*current_song);
      break;
    }
    case 'A':
    case 'a': {
      std::cin.clear();
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      std::string song_name, artist;
      int rating;
      std::cout << "Enter song name: ";
      std::getline(std::cin, song_name);
      std::cout << "Enter artist name: ";
      std::getline(std::cin, artist);
      std::cout << "Enter the rating of the song: ";
      std::cin >> rating;
      current_song =
          playlist.insert(current_song, Song{song_name, artist, rating});
      play_current_song(*current_song);
      break;
    }
    case 'L':
    case 'l':
      display_playlist(playlist, *current_song);
      break;
    case 'Q':
    case 'q':
      running = false;
      break;
    default:
      std::cout << "Invalid choice\n";
      break;
    }
  }

  std::cout << "Thanks for listening!" << std::endl;
  return 0;
}