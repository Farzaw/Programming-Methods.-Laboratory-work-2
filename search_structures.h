#pragma once

#include <algorithm>
#include <cstddef>
#include <string>
#include <utility>
#include <vector>

#include "flight.h"

/**
 * @file search_structures.h
 * @brief Алгоритмы и структуры поиска по ключу airline.
 *
 * Все методы поиска возвращают @c std::vector<Flight> — полный набор
 * записей с совпадающим ключом (дубликаты ключей допустимы).
 */

// ─── Линейный поиск ───────────────────────────────────────────────────────────

/**
 * @brief Линейный поиск всех рейсов заданной авиакомпании.
 *
 * Последовательно просматривает весь вектор и собирает совпадения.
 *
 * @param data Контейнер с рейсами.
 * @param key  Название авиакомпании (ключ поиска).
 * @return Вектор всех найденных рейсов; пустой, если совпадений нет.
 *
 * @complexity Средний и худший случай: O(n).
 */
inline std::vector<Flight> linearSearch(const std::vector<Flight>& data,
                                        const std::string& key) {
    std::vector<Flight> found;
    found.reserve(data.size() / 10 + 1);
    for (const Flight& flight : data) {
        if (flight.airline == key) {
            found.push_back(flight);
        }
    }
    return found;
}

// ─── Бинарное дерево поиска (BST) ────────────────────────────────────────────

/**
 * @class BinarySearchTree
 * @brief Бинарное дерево поиска по строковому ключу airline.
 *
 * Каждый узел хранит один уникальный ключ и вектор всех рейсов с этим ключом.
 */
class BinarySearchTree {
public:
    /**
     * @struct Node
     * @brief Узел BST: ключ и список значений с одинаковым ключом.
     */
    struct Node {
        std::string key;              ///< Ключ (airline)
        std::vector<Flight> values;     ///< Все рейсы с данным ключом
        Node* left;                   ///< Левое поддерево (ключи меньше)
        Node* right;                  ///< Правое поддерево (ключи больше)

        /**
         * @brief Конструктор узла с одним начальным значением.
         * @param k Ключ.
         * @param value Первый рейс для данного ключа.
         */
        explicit Node(std::string k, const Flight& value)
            : key(std::move(k)), values{value}, left(nullptr), right(nullptr) {}
    };

    /** @brief Создаёт пустое дерево. */
    BinarySearchTree() : root_(nullptr) {}

    /** @brief Освобождает память всех узлов. */
    ~BinarySearchTree() { clear(); }

    BinarySearchTree(const BinarySearchTree&) = delete;
    BinarySearchTree& operator=(const BinarySearchTree&) = delete;

    /**
     * @brief Вставляет рейс в дерево по ключу airline.
     * @param flight Добавляемый рейс.
     */
    void insert(const Flight& flight) { root_ = insertNode(root_, flight); }

    /**
     * @brief Находит все рейсы с заданным ключом.
     * @param key Название авиакомпании.
     * @return Вектор найденных рейсов.
     *
     * @complexity Средний случай O(log n), худший O(n) при вырожденном дереве.
     */
    std::vector<Flight> search(const std::string& key) const {
        const Node* node = findNode(root_, key);
        return node ? node->values : std::vector<Flight>{};
    }

    /** @brief Удаляет все узлы дерева. */
    void clear() {
        destroy(root_);
        root_ = nullptr;
    }

private:
    Node* root_;

    /**
     * @brief Рекурсивная вставка узла.
     * @param node Текущий корень поддерева.
     * @param flight Вставляемый рейс.
     * @return Новый корень поддерева.
     */
    static Node* insertNode(Node* node, const Flight& flight) {
        if (!node) {
            return new Node(flight.airline, flight);
        }
        const int cmp = compareKeys(flight.airline, node->key);
        if (cmp < 0) {
            node->left = insertNode(node->left, flight);
        } else if (cmp > 0) {
            node->right = insertNode(node->right, flight);
        } else {
            node->values.push_back(flight);
        }
        return node;
    }

    /**
     * @brief Рекурсивный поиск узла по ключу.
     * @param node Текущий узел.
     * @param key Искомый ключ.
     * @return Указатель на узел или nullptr.
     */
    static const Node* findNode(const Node* node, const std::string& key) {
        if (!node) return nullptr;
        const int cmp = compareKeys(key, node->key);
        if (cmp < 0) return findNode(node->left, key);
        if (cmp > 0) return findNode(node->right, key);
        return node;
    }

    /** @brief Рекурсивное удаление поддерева. */
    static void destroy(Node* node) {
        if (!node) return;
        destroy(node->left);
        destroy(node->right);
        delete node;
    }
};

// ─── Красно-чёрное дерево (RB-Tree) ─────────────────────────────────────────

/**
 * @enum Color
 * @brief Цвет узла красно-чёрного дерева.
 */
enum class Color { RED, BLACK };

/**
 * @class RedBlackTree
 * @brief Сбалансированное красно-чёрное дерево поиска по ключу airline.
 *
 * После каждой вставки вызывается @ref fixInsert для восстановления
 * инвариантов RB-дерева (повороты и перекраска).
 */
class RedBlackTree {
public:
    /**
     * @struct Node
     * @brief Узел RB-дерева.
     */
    struct Node {
        std::string key;              ///< Ключ (airline)
        std::vector<Flight> values;     ///< Все рейсы с данным ключом
        Color color;                  ///< Цвет узла (RED / BLACK)
        Node* left;                   ///< Левый потомок
        Node* right;                  ///< Правый потомок
        Node* parent;                 ///< Родительский узел

        /**
         * @brief Конструктор нового красного узла-листа.
         * @param k Ключ.
         * @param value Первый рейс.
         */
        Node(std::string k, const Flight& value)
            : key(std::move(k)),
              values{value},
              color(Color::RED),
              left(nullptr),
              right(nullptr),
              parent(nullptr) {}
    };

    /** @brief Создаёт пустое RB-дерево. */
    RedBlackTree() : root_(nullptr) {}

    /** @brief Освобождает память всех узлов. */
    ~RedBlackTree() { clear(); }

    RedBlackTree(const RedBlackTree&) = delete;
    RedBlackTree& operator=(const RedBlackTree&) = delete;

    /**
     * @brief Вставляет рейс и балансирует дерево.
     * @param flight Добавляемый рейс.
     */
    void insert(const Flight& flight) {
        Node* node = insertNode(flight);
        if (node) {
            fixInsert(node);
            root_->color = Color::BLACK;
        }
    }

    /**
     * @brief Находит все рейсы с заданным ключом.
     * @param key Название авиакомпании.
     * @return Вектор найденных рейсов.
     *
     * @complexity Средний и худший случай O(log n).
     */
    std::vector<Flight> search(const std::string& key) const {
        const Node* node = findNode(root_, key);
        return node ? node->values : std::vector<Flight>{};
    }

    /** @brief Удаляет все узлы. */
    void clear() {
        destroy(root_);
        root_ = nullptr;
    }

private:
    Node* root_;

    /**
     * @brief Вставка узла по правилам BST; новый узел всегда красный.
     * @param flight Вставляемый рейс.
     * @return Указатель на вставленный/обновлённый узел или nullptr при дубликате ключа.
     */
    Node* insertNode(const Flight& flight) {
        Node* parent = nullptr;
        Node* current = root_;

        while (current) {
            parent = current;
            const int cmp = compareKeys(flight.airline, current->key);
            if (cmp < 0) {
                current = current->left;
            } else if (cmp > 0) {
                current = current->right;
            } else {
                current->values.push_back(flight);
                return nullptr;
            }
        }

        Node* node = new Node(flight.airline, flight);
        node->parent = parent;

        if (!parent) {
            root_ = node;
        } else if (compareKeys(flight.airline, parent->key) < 0) {
            parent->left = node;
        } else {
            parent->right = node;
        }
        return node;
    }

    /**
     * @brief Левый поворот вокруг узла @p x.
     *
     * Используется при нарушении баланса, когда «линия» или «треугольник»
     * образуются справа. Поднимает правого потомка @p x на место @p x.
     *
     * @param x Узел, вокруг которого выполняется поворот.
     */
    void leftRotate(Node* x) {
        Node* y = x->right;
        x->right = y->left;
        if (y->left) {
            y->left->parent = x;
        }
        y->parent = x->parent;
        if (!x->parent) {
            root_ = y;
        } else if (x == x->parent->left) {
            x->parent->left = y;
        } else {
            x->parent->right = y;
        }
        y->left = x;
        x->parent = y;
    }

    /**
     * @brief Правый поворот — зеркальная операция @ref leftRotate.
     * @param y Узел, вокруг которого выполняется поворот.
     */
    void rightRotate(Node* y) {
        Node* x = y->left;
        y->left = x->right;
        if (x->right) {
            x->right->parent = y;
        }
        x->parent = y->parent;
        if (!y->parent) {
            root_ = x;
        } else if (y == y->parent->left) {
            y->parent->left = x;
        } else {
            y->parent->right = x;
        }
        x->right = y;
        y->parent = x;
    }

    /**
     * @brief Восстанавливает инварианты RB-дерева после вставки красного узла @p z.
     *
     * Обрабатываемые случаи (родитель @p z — левый потомок дедушки; симметрично и для правого):
     * 1. Дядя красный → перекраска (родитель, дядя → чёрные; дед → красный), подъём @p z.
     * 2. @p z — правый потомок (треугольник) → leftRotate на родителе.
     * 3. @p z — левый потомок (линия) → перекраска + rightRotate на дедушке.
     *
     * @param z Только что вставленный красный узел.
     */
    void fixInsert(Node* z) {
        while (z->parent && z->parent->color == Color::RED) {
            Node* parent = z->parent;
            Node* grandparent = parent->parent;

            if (parent == grandparent->left) {
                Node* uncle = grandparent->right;
                if (uncle && uncle->color == Color::RED) {
                    parent->color = Color::BLACK;
                    uncle->color = Color::BLACK;
                    grandparent->color = Color::RED;
                    z = grandparent;
                } else {
                    if (z == parent->right) {
                        z = parent;
                        leftRotate(z);
                        parent = z->parent;
                        grandparent = parent ? parent->parent : nullptr;
                    }
                    if (parent) {
                        parent->color = Color::BLACK;
                        if (grandparent) {
                            grandparent->color = Color::RED;
                            rightRotate(grandparent);
                        }
                    }
                }
            } else {
                Node* uncle = grandparent->left;
                if (uncle && uncle->color == Color::RED) {
                    parent->color = Color::BLACK;
                    uncle->color = Color::BLACK;
                    grandparent->color = Color::RED;
                    z = grandparent;
                } else {
                    if (z == parent->left) {
                        z = parent;
                        rightRotate(z);
                        parent = z->parent;
                        grandparent = parent ? parent->parent : nullptr;
                    }
                    if (parent) {
                        parent->color = Color::BLACK;
                        if (grandparent) {
                            grandparent->color = Color::RED;
                            leftRotate(grandparent);
                        }
                    }
                }
            }
        }
        if (root_) {
            root_->color = Color::BLACK;
        }
    }

    /** @brief Поиск узла по ключу. */
    static const Node* findNode(const Node* node, const std::string& key) {
        while (node) {
            const int cmp = compareKeys(key, node->key);
            if (cmp < 0) {
                node = node->left;
            } else if (cmp > 0) {
                node = node->right;
            } else {
                return node;
            }
        }
        return nullptr;
    }

    /** @brief Рекурсивное удаление поддерева. */
    static void destroy(Node* node) {
        if (!node) return;
        destroy(node->left);
        destroy(node->right);
        delete node;
    }
};

// ─── Хэш-таблица (метод цепочек) ────────────────────────────────────────────

/**
 * @class HashTable
 * @brief Хэш-таблица с полиномиальной хэш-функцией и разрешением коллизий цепочками.
 */
class HashTable {
public:
    using BucketChain = std::vector<std::pair<std::string, std::vector<Flight>>>;

    /**
     * @brief Создаёт таблицу с заданным числом корзин.
     * @param bucketCount Количество корзин (рекомендуется простое число).
     */
    explicit HashTable(std::size_t bucketCount = 10007)
        : buckets_(bucketCount), collisionCount_(0) {}

    /**
     * @brief Вставляет рейс в таблицу.
     *
     * При попадании в непустую корзину увеличивает счётчик коллизий.
     *
     * @param flight Добавляемый рейс.
     */
    void insert(const Flight& flight) {
        const std::size_t index = hash(flight.airline);
        BucketChain& chain = buckets_[index];

        if (!chain.empty()) {
            ++collisionCount_;
        }

        for (auto& entry : chain) {
            if (entry.first == flight.airline) {
                entry.second.push_back(flight);
                return;
            }
        }
        chain.emplace_back(flight.airline, std::vector<Flight>{flight});
    }

    /**
     * @brief Находит все рейсы с заданным ключом.
     * @param key Название авиакомпании.
     * @return Вектор найденных рейсов.
     *
     * @complexity Средний случай O(1 + α), худший O(n) при всех ключах в одной корзине.
     */
    std::vector<Flight> search(const std::string& key) const {
        const std::size_t index = hash(key);
        for (const auto& entry : buckets_[index]) {
            if (entry.first == key) {
                return entry.second;
            }
        }
        return {};
    }

    /**
     * @brief Возвращает общее число коллизий, накопленное при вставках.
     * @return Число коллизий.
     */
    std::size_t collisionCount() const { return collisionCount_; }

    /** @brief Число корзин таблицы. */
    std::size_t bucketCount() const { return buckets_.size(); }

    /** @brief Очищает таблицу и сбрасывает счётчик коллизий. */
    void clear() {
        for (BucketChain& chain : buckets_) {
            chain.clear();
        }
        collisionCount_ = 0;
    }

private:
    std::vector<BucketChain> buckets_;
    std::size_t collisionCount_;

    /**
     * @brief Полиномиальная хэш-функция для строк (основание p = 31).
     *
     * h = (s[0] + s[1]*p + s[2]*p² + …) mod m, где m — число корзин.
     *
     * @param key Строка-ключ.
     * @return Индекс корзины в диапазоне [0, bucketCount).
     */
    std::size_t hash(const std::string& key) const {
        const std::size_t p = 31;
        const std::size_t m = buckets_.size();
        std::size_t hashValue = 0;
        std::size_t power = 1;
        for (unsigned char ch : key) {
            hashValue = (hashValue + static_cast<std::size_t>(ch + 1) * power) % m;
            power = (power * p) % m;
        }
        return hashValue;
    }
};

/**
 * @brief Возвращает простое число корзин, близкое к 2 * expectedSize.
 * @param expectedSize Ожидаемое число элементов.
 * @return Рекомендуемый размер таблицы.
 */
inline std::size_t recommendedBucketCount(std::size_t expectedSize) {
    std::size_t candidate = std::max<std::size_t>(101, expectedSize * 2);
    while (true) {
        bool prime = candidate >= 2;
        for (std::size_t d = 2; d * d <= candidate; ++d) {
            if (candidate % d == 0) {
                prime = false;
                break;
            }
        }
        if (prime) return candidate;
        ++candidate;
    }
}
