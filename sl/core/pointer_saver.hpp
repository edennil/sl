#pragma once

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <memory>
#include <exception>

namespace sl
{
        
    namespace detail
    {
        
        class pointer_saver_out
        {
        public:
            ~pointer_saver_out() = default;
            pointer_saver_out() = default;

            template<typename T>
            bool contains(T *ptr)
            {
                auto it = pointers_.find(static_cast<void *>(ptr));
                if(it != pointers_.end())
                {
                    return true;
                }
                return false;
            }

            template<typename T>
            size_t add(T *ptr)
            {
                auto memory = static_cast<void *>(ptr);
                auto it = pointers_.find(memory);
                if(it == pointers_.end())
                {
                    auto size = positions_.size();
                    positions_.emplace_back(memory);
                    pointers_.emplace(memory, size);
                    return size;
                }
                return it->second;
            }

            template<typename T>
            size_t add(const std::shared_ptr<T> &ptr)
            {
                auto memory = static_cast<void *>(ptr.get());
                auto it = pointers_.find(memory);
                if(it == pointers_.end())
                {
                    auto size = positions_.size();
                    positions_.emplace_back(memory);
                    pointers_.emplace(memory, size);
                    return size;
                }
                return it->second;   
            }

            template<typename T>
            size_t add(const std::unique_ptr<T> &ptr)
            {
                auto memory = static_cast<void *>(ptr.get());
                auto it = pointers_.find(memory);
                if(it == pointers_.end())
                {
                    auto size = positions_.size();
                    positions_.emplace_back(memory);
                    pointers_.emplace(memory, size);
                    return size;
                }
                return it->second;   
            }

            template<typename T>
            size_t id(T *ptr)
            {
                auto it = pointers_.find(static_cast<void *>(ptr));
                if(it != pointers_.end())
                {
                    return it->second;
                }
                return -1;
            }

            template<typename T>
            size_t id(const std::shared_ptr<T> &ptr)
            {
                auto it = pointers_.find(static_cast<void *>(ptr.get()));
                if(it != pointers_.end())
                {
                    return it->second;
                }
                return -1;
            }

            template<typename T>
            size_t id(std::unique_ptr<T> &ptr)
            {
                auto it = pointers_.find(static_cast<void *>(ptr.get()));
                if(it != pointers_.end())
                {
                    return it->second;
                }
                return -1;
            }

        protected:

            std::unordered_map<void *, size_t> pointers_;
            std::vector<void *> positions_;
        };


        class pointer_saver_in
        {
        public:

            ~pointer_saver_in() = default;
            pointer_saver_in() = default;

            template<typename T>
            int add(T *ptr)
            {
                auto memory = static_cast<void *>(ptr);
                auto it = pointers_.find(memory);
                if(it == pointers_.end())
                {
                    pointers_.emplace(memory, positions_.size());
                    positions_.emplace_back(memory, nullptr);
                    return -1;
                }
                return it->second;
            }

            template<typename T>
            int add(const std::shared_ptr<T> &ptr)
            {
                auto memory = static_cast<void *>(ptr.get());
                auto it = pointers_.find(memory);
                if(it == pointers_.end())
                {
                    pointers_.emplace(memory, positions_.size());
                    positions_.emplace_back(memory, std::static_pointer_cast<void>(ptr));
                    return -1;
                }
                return it->second;
            }

            template<typename T>
            int add(const std::unique_ptr<T> &ptr)
            {
                auto memory = static_cast<void *>(ptr.get());
                auto it = pointers_.find(memory);
                if(it == pointers_.end())
                {
                    pointers_.emplace(memory, positions_.size());
                    unique_ptr_.emplace(positions_.size());
                    positions_.emplace_back(memory, nullptr);
                    return -1;
                }
                return it->second;
            }

            template<typename T>
            int add(size_t position, T *ptr)
            {
                auto memory = static_cast<void *>(ptr);
                auto it = pointers_.find(memory);
                if(it == pointers_.end())
                {
                    if(position + 1 > positions_.size())
                    {
                        positions_.resize(position + 1);
                    }
                    positions_[position] = std::make_pair(memory, nullptr);
                    pointers_.emplace(memory, position);
                    return -1;
                }
                throw std::exception();
            }

            template<typename T>
            int add(size_t position, const std::shared_ptr<T> &ptr)
            {
                auto memory = static_cast<void *>(ptr.get());
                auto it = pointers_.find(memory);
                if(it == pointers_.end())
                {
                    if(position + 1 > positions_.size())
                    {
                        positions_.resize(position + 1);
                    }
                    positions_[position] = std::make_pair(memory, nullptr);
                    pointers_.emplace(memory, position);
                    return -1;
                }
                throw std::exception();
            }

            template<typename T>
            int add(size_t position, const std::unique_ptr<T> &ptr)
            {
                auto memory = static_cast<void *>(ptr.get());
                auto it = pointers_.find(memory);
                if(it == pointers_.end())
                {
                    if(position + 1 > positions_.size())
                    {
                        positions_.resize(position + 1);
                    }
                    positions_[position] = std::make_pair(memory, nullptr);
                    unique_ptr_.emplace(position);
                    pointers_.emplace(memory, position);
                    return -1;
                }
                throw std::exception();
            }

            template<typename T>
            void get_obj(size_t i, T *&obj)
            {
                if(i < positions_.size())
                {
                    obj = static_cast<T*>(positions_[i].first);
                }
                else
                {
                    obj = nullptr;
                }
            }

            template<typename T>
            void get_obj(size_t i, std::shared_ptr<T> &obj)
            {
                if(i < positions_.size())
                {
                    if(positions_[i].second == nullptr && positions_[i].first != nullptr)
                    {
                        auto it = unique_ptr_.find(i);
                        if(it != unique_ptr_.end())
                        {
                            throw std::runtime_error("This pointer is used as a unique_ptr");
                        }
                        auto tmp = std::shared_ptr<T>(static_cast<T *>(positions_[i].first));
                        positions_[i].second = std::static_pointer_cast<void>(tmp);
                        obj = std::move(tmp);
                    }
                    else
                    {
                        obj = static_cast<T *>(positions_[i].first);
                    }
                }
            }

            template<typename T>
            void get_obj(size_t i, std::unique_ptr<T> &obj)
            {
                if(i < positions_.size())
                {
                    auto it = unique_ptr_.find(i);
                    if(it != unique_ptr_.end())
                    {
                        if(positions_[i].second)
                        {
                            throw std::runtime_error("We can't create a unique_ptr using a shared_ptr");
                        }
                        obj = std::make_unique<T>(static_cast<T *>(positions_[i].first));
                        unique_ptr_.emplace(i);
                    }
                    else
                    {
                        throw std::runtime_error("only one unique_ptr");
                    }
                }
            }

        protected:

            std::unordered_map<void *, size_t> pointers_;
            std::vector<std::pair<void *, std::shared_ptr<void>>> positions_;
            std::unordered_set<size_t> unique_ptr_;
        };

    } // namespace detail
    
} // namespace sl
