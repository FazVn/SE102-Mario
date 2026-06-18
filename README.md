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

## Gợi ý chia việc cho 3 người

### Người 1: SceneManager và kiến trúc lõi

Phụ trách:

```text
Scenes/IScene
Scenes/SceneManager
Scenes/SceneFactory
Scenes/MenuScene
Scenes/PlayScene
Scenes/GameOverScene
Core/Game
World/EntityManager
```

Mục tiêu tuần 1:

```text
MenuScene → PlayScene → GameOverScene
có thể chuyển scene an toàn
không đổi scene trực tiếp giữa vòng lặp Update
```

### Người 2: Renderer, camera và asset pipeline

Phụ trách:

```text
Core/Renderer
Core/RenderOptions
Core/Sprite
Core/Animation
Core/Camera
Resources/definitions
Resources/textures
```

Mục tiêu tuần 1:

```text
mọi sprite đi qua Renderer
có thể sửa flip/scale/alpha/camera ở một chỗ
có sprite placeholder cho Mario, Goomba, ground, coin
```

### Người 3: Gameplay prototype

Phụ trách:

```text
Objects/GameObject
Objects/Player/Mario
Objects/Enemies/Goomba
Objects/Blocks/Brick
Objects/Items/Coin
World/CollisionSystem
World/LevelLoader
World/ObjectFactory
```

Mục tiêu tuần 1:

```text
Mario chạy và nhảy
Mario va chạm nền
Goomba đi ngang
Mario chạm Goomba thì xử lý đúng
map demo load được
```

## Luồng render dự kiến

```text
PlayScene::Render()
→ EntityManager::RenderAll()
→ Mario::Render()
→ Animation::Render()
→ Renderer::DrawSprite()
→ DirectX draw call
```

### Render call chain giai đoạn đầu

Ở sample cũ của thầy, lệnh vẽ thường đi theo hướng:

```text
main.cpp::Render()
→ CGame::Draw()
→ ID3DX10Sprite::DrawSpritesImmediate()
→ SwapChain::Present()
```

Trong project mới, nhóm mình gom phần đó vào `Core/Renderer`:

```text
Game::Render()
→ SceneManager::Render()
→ PlayScene::Render()
→ EntityManager::RenderAll()
→ GameObject::Render()
→ Sprite/Animation chọn frame cần vẽ
→ Renderer::DrawSprite()
→ ID3DX10Sprite::DrawSpritesImmediate()
```

Ý tưởng chính: `GameObject`, `Mario`, `Goomba`, `Coin` không gọi DirectX trực tiếp. Các object chỉ nói "tôi muốn vẽ sprite này ở tọa độ này"; `Renderer` chịu trách nhiệm biến yêu cầu đó thành draw call.

Sau khi refactor, nếu cần sửa render sprite thì ưu tiên xem:

```text
Core/Renderer.cpp
Core/RenderOptions.h
Core/Sprite.cpp
Core/Animation.cpp
```

## Luồng update dự kiến

```text
Game::Update()
→ SceneManager::GetCurrentScene()
→ PlayScene::Update()
→ EntityManager::UpdateAll()
→ CollisionSystem::Resolve()
→ EntityManager::RemoveDeletedEntities()
→ Camera::Follow()
→ SceneManager::ApplyPendingChange()
```

## Luồng chuyển scene dự kiến

```text
Object hoặc PlayScene yêu cầu chuyển scene
→ SceneManager::RequestChange(sceneId)
→ kết thúc frame
→ SceneManager::ApplyPendingChange()
→ Unload scene cũ
→ Load scene mới
```

Không nên xóa scene hoặc load scene mới ngay bên trong collision/update của object.

## File level mẫu

`Resources/levels/demo_level.txt` hiện chỉ là placeholder.  
Có thể dùng format dự kiến như sau:

```text
[SETTINGS]
WIDTH 2400
HEIGHT 480
START_X 100
START_Y 300

[OBJECTS]
MARIO 100 300
PLATFORM 0 400 32 32 40
BRICK 500 280
QUESTION_BLOCK 532 280
GOOMBA 700 368
GOOMBA 1100 368
PORTAL 2200 0 WIN_SCENE
```

## Thứ tự triển khai khuyến nghị

### Bước 1

Tạo project Visual Studio và add toàn bộ file vào solution.

### Bước 2

Copy phần DirectX/window/input cơ bản từ sample của thầy vào `Core`.

### Bước 3

Làm `IScene`, `SceneManager`, `SceneFactory`.

### Bước 4

Làm `MenuScene` và `PlayScene` rỗng để test chuyển scene.

### Bước 5

Làm `Renderer` wrapper để mọi sprite vẽ qua một chỗ.

### Bước 6

Làm `Mario`, `Goomba`, `LevelLoader`, `ObjectFactory`.

### Bước 7

Làm collision và map demo.

## Quy tắc Git

Khuyến nghị tạo các branch:

```text
main
develop
feature/scene-manager
feature/renderer
feature/gameplay
```

Cuối mỗi ngày:

```text
pull develop
build project
chạy demo
merge phần đã ổn vào develop
```

Không nên để đến cuối tuần mới merge.

## Lưu ý

- Template này chưa chứa `.sln` vì nhóm có thể tạo solution theo Visual Studio đang dùng.
- Các file `.cpp` đang rỗng.
- Các file `.h` cũng chưa có nội dung để nhóm tự quyết định interface cuối cùng.
- Có thể copy từng phần từ sample của thầy vào cấu trúc này, nhưng nên tránh copy nguyên `PlayScene.cpp` cũ rồi tiếp tục mở rộng nó.
