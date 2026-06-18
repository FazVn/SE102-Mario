# MarioGame Reorganized Template

Đây là bộ khung thư mục dự kiến sau khi refactor lại project Mario dựa trên sample của thầy.

Template này **chưa có code gameplay**.  
Các file `.cpp` đang được để rỗng để nhóm tự triển khai dần.

## Mục tiêu của cấu trúc mới

Cấu trúc này tách project thành nhiều module nhỏ để tránh tình trạng sửa một phần như render sprite, scene hoặc collision rồi làm lệch đồng bộ với nhiều file khác.

Nguyên tắc chính:

```text
Game chỉ chạy vòng lặp chính
SceneManager chỉ quản lý chuyển scene
PlayScene chỉ điều phối màn chơi
Renderer chỉ vẽ sprite
EntityManager chỉ quản lý object
LevelLoader chỉ đọc level
ObjectFactory chỉ tạo object
CollisionSystem chỉ xử lý va chạm
Object chỉ chứa logic riêng của nó
```

## Cấu trúc thư mục

```text
MarioGame/
├── Core/
│   ├── Game
│   ├── Input
│   ├── Renderer
│   ├── Camera
│   ├── Texture
│   ├── Sprite
│   └── Animation
│
├── Scenes/
│   ├── IScene
│   ├── SceneManager
│   ├── SceneFactory
│   ├── MenuScene
│   ├── PlayScene
│   ├── GameOverScene
│   └── WinScene
│
├── World/
│   ├── Level
│   ├── LevelLoader
│   ├── TileMap
│   ├── EntityManager
│   ├── ObjectFactory
│   └── CollisionSystem
│
├── Objects/
│   ├── GameObject
│   ├── Player/
│   ├── Enemies/
│   ├── Blocks/
│   ├── Items/
│   └── Environment/
│
├── Resources/
│   ├── textures/
│   ├── definitions/
│   └── levels/
│
└── Tests/
    ├── test_cases.md
    └── known_bugs.md
```

## Vai trò từng module

### Core

Chứa các phần nền tảng của game:

- `Game`: vòng lặp chính.
- `Input`: xử lý bàn phím.
- `Renderer`: điểm tập trung để sửa render sprite.
- `Camera`: camera theo Mario.
- `Texture`, `Sprite`, `Animation`: quản lý hình ảnh và animation.

### Scenes

Chứa hệ thống màn hình/game scene:

- `IScene`: interface chung cho mọi scene.
- `SceneManager`: quản lý scene hiện tại.
- `SceneFactory`: tạo scene theo ID.
- `MenuScene`: màn hình menu.
- `PlayScene`: màn chơi chính.
- `GameOverScene`: màn thua.
- `WinScene`: màn thắng.

### World

Chứa các hệ thống phục vụ màn chơi:

- `Level`: dữ liệu một màn chơi.
- `LevelLoader`: đọc file level.
- `TileMap`: quản lý map tile.
- `EntityManager`: quản lý danh sách object.
- `ObjectFactory`: tạo object từ file level.
- `CollisionSystem`: xử lý va chạm.

### Objects

Chứa các object trong game:

- `GameObject`: class gốc.
- `Player/Mario`: nhân vật chính.
- `Enemies`: Goomba, Koopa.
- `Blocks`: Brick, QuestionBlock.
- `Items`: Coin, Mushroom.
- `Environment`: Platform, Pipe, Portal.

### Resources

Chứa tài nguyên:

- `textures/`: ảnh sprite sheet.
- `definitions/`: file định nghĩa texture, sprite, animation.
- `levels/`: file map/level.

### Tests

Chứa tài liệu kiểm thử:

- `test_cases.md`: danh sách test case.
- `known_bugs.md`: danh sách lỗi còn tồn tại.


- Template này chưa chứa `.sln` vì nhóm có thể tạo solution theo Visual Studio đang dùng.
- Các file `.cpp` đang rỗng.
- Các file `.h` cũng chưa có nội dung để nhóm tự quyết định interface cuối cùng.
- Có thể copy từng phần từ sample của thầy vào cấu trúc này, nhưng nên tránh copy nguyên `PlayScene.cpp` cũ rồi tiếp tục mở rộng nó.
