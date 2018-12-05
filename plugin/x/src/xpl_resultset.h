/*
 * Copyright (c) 2017, 2018, Oracle and/or its affiliates. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, version 2.0,
 * as published by the Free Software Foundation.
 *
 * This program is also distributed with certain software (including
 * but not limited to OpenSSL) that is licensed under separate terms,
 * as designated in a particular file or component or in included license
 * documentation.  The authors of MySQL hereby grant you an additional
 * permission to link the program and your derivative works with the
 * separately licensed software that they have included with MySQL.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License, version 2.0, for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA
 */

#ifndef PLUGIN_X_SRC_XPL_RESULTSET_H_
#define PLUGIN_X_SRC_XPL_RESULTSET_H_

#include <vector>

#include "plugin/x/ngs/include/ngs/interface/notice_output_queue_interface.h"
#include "plugin/x/ngs/include/ngs/interface/resultset_interface.h"
#include "plugin/x/src/buffering_command_delegate.h"
#include "plugin/x/src/streaming_command_delegate.h"

namespace xpl {

class Process_resultset : public ngs::Resultset_interface {
 public:
  using Row = Callback_command_delegate::Row_data;
  using Field = Callback_command_delegate::Field_value;
  using Field_list = std::vector<Field *>;
  Process_resultset()
      : m_callback_delegate(std::bind(&Process_resultset::start_row, this),
                            std::bind(&Process_resultset::end_row, this,
                                      std::placeholders::_1)) {}
  ngs::Command_delegate &get_callbacks() override {
    return m_callback_delegate;
  }
  const Info &get_info() const override {
    return m_callback_delegate.get_info();
  }

 protected:
  virtual Row *start_row() = 0;
  virtual bool end_row(Row *) = 0;

 private:
  Callback_command_delegate m_callback_delegate;
};

class Empty_resultset : public ngs::Resultset_interface {
 public:
  Empty_resultset()
      : m_callback_delegate(Callback_command_delegate::Start_row_callback(),
                            Callback_command_delegate::End_row_callback()) {}
  ngs::Command_delegate &get_callbacks() override {
    return m_callback_delegate;
  }
  const Info &get_info() const override {
    return m_callback_delegate.get_info();
  }

 private:
  Callback_command_delegate m_callback_delegate;
};

class Collect_resultset : public ngs::Resultset_interface {
 public:
  using Row_list = Buffering_command_delegate::Resultset;
  using Row = Buffering_command_delegate::Row_data;
  using Field = Buffering_command_delegate::Field_value;
  using Field_types = Buffering_command_delegate::Field_types;

  ngs::Command_delegate &get_callbacks() override {
    return m_buffering_delegate;
  }

  const Info &get_info() const override {
    return m_buffering_delegate.get_info();
  }

  void reset() { m_buffering_delegate.reset(); }

  const Row_list &get_row_list() const {
    return m_buffering_delegate.get_resultset();
  }
  const Field_types &get_field_types() const {
    return m_buffering_delegate.get_field_types();
  }

 protected:
  void set_row_list(const Row_list &list) {
    m_buffering_delegate.set_resultset(list);
  }
  void set_field_types(const Field_types &field_types) {
    m_buffering_delegate.set_field_types(field_types);
  }

 private:
  Buffering_command_delegate m_buffering_delegate;
};

class Streaming_resultset : public ngs::Resultset_interface {
 public:
  Streaming_resultset(ngs::Protocol_encoder_interface *proto,
                      ngs::Notice_output_queue_interface *notice_queue,
                      const bool compact_metadata)
      : m_streaming_delegate(proto, notice_queue) {
    m_streaming_delegate.set_compact_metadata(compact_metadata);
  }
  ngs::Command_delegate &get_callbacks() override {
    return m_streaming_delegate;
  }
  const Info &get_info() const override {
    return m_streaming_delegate.get_info();
  }

 private:
  Streaming_command_delegate m_streaming_delegate;
};

}  // namespace xpl

#endif  // PLUGIN_X_SRC_XPL_RESULTSET_H_
