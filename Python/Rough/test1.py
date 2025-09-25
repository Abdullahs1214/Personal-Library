import os

base_dir = "/Users/Goku/Desktop/anime/Dr Stone"  # <-- Change this to your specific folder

# Get a list of files in the folder that start with 'getvid'
video_files = [f for f in os.listdir(base_dir)
               if f.lower().startswith("getvid") and f.lower().endswith((".mp4", ".mkv", ".avi"))]

# Sort them alphabetically
video_files.sort()

# Rename each file sequentially
for idx, filename in enumerate(video_files, start=1):
  name, ext = os.path.splitext(filename)
  new_name = f"DR stone - Episode {str(idx).zfill(2)}{ext}"
  old_path = os.path.join(base_dir, filename)
  new_path = os.path.join(base_dir, new_name)

  if old_path != new_path:
    os.rename(old_path, new_path)
    print(f"Renamed:\n  {old_path}\n→ {new_path}")
